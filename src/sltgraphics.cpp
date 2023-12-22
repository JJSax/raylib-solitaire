#include "sltgraphics.hpp"

#include <utility>

namespace solitaire {
    GraphicalGame::GraphicalGame() {
        std::string basePath(CARD_TEXTURE_PATH_PREFIX);
        for (Suit s = Suit::FIRST; s < Suit::END; s++) {
            std::stringstream suitPath;
            suitPath << basePath;
            suitPath << suitToChar(s);
            for (Face f = Face::FIRST; f < Face::END; f++) {
                std::stringstream cardPath;
                cardPath << suitPath.str();
                cardPath << '/' << faceToChar(f) << ".png";

                Texture tex = LoadTexture(cardPath.str().c_str());
                this->cardTextures.emplace(std::make_pair(s, f), tex);
            }
        }
        auto backTexturePath = basePath + CARD_BACK_TEXTURE_PATH_SUFFIX;
        this->cardBackTexture = LoadTexture(backTexturePath.c_str());

        // TODO calculate resolution
        this->actualResolution = TARGET_RESOLUTION;
    }

    GraphicalGame::~GraphicalGame() {
        for (auto entry : this->cardTextures) {
            Texture tex = entry.second;
            UnloadTexture(tex);
        }
        UnloadTexture(this->cardBackTexture);
        delete this->game;
    }

    GraphicalGame::GraphicalGame(std::minstd_rand::result_type seed) {
        auto rand = std::minstd_rand(seed);
        this->game = Game::createAndDealGame(rand);
    }

    int stackPxSize(int nFaceDown, int nFaceUp, int cardHeight) {
        return (nFaceDown) * FACE_DOWN_STACKED_DISPLACEMENT +
            (nFaceUp - 1) * STACKED_DISPLACEMENT +
            MIN_CARD_SHOWN_SIZE;
    }

    void GraphicalGame::calculateBounds() {
        auto const windowPadding = SMALL_SPACING;
        float tallestPossibleTableau = stackPxSize(
            NUM_TABLEAUS - 1,
            static_cast<int>(Face::END),
            this->cardHeight()
        );
        // float stockAndWasteWidth = this->cardWidth() + 2 * SMALL_SPACING;
        float stockY = (this->actualResolution.y - this->cardHeight()) / 2;
        this->stockPosition = { windowPadding, stockY };
        this->wasteStart = { this->stockPosition.x + this->cardWidth() + TINY_SPACING, stockY };
        Vector2 wasteEnd = { this->wasteStart.x + this->cardWidth(), stockY + this->cardHeight() };

        // 1.
        // this->tableauStart = { wasteEnd.x/*  + SMALL_SPACING */, tallestPossibleTableau };

        // 2.
        // auto tableausWidth = NUM_TABLEAUS * this->cardWidth() + (NUM_TABLEAUS - 1) * TINY_SPACING;
        // float tableauMargin = (this->windowWidth() - wasteEnd.x - tableausWidth) / 2.0f;
        // this->tableauStart = { tableauMargin, tallestPossibleTableau };

        // 3.
        auto tableausWidth = NUM_TABLEAUS * this->cardWidth() + (NUM_TABLEAUS - 1) * TINY_SPACING;
        this->tableauStart = Vector2 { this->actualResolution.x - windowPadding - tableausWidth, tallestPossibleTableau };
    }

    void GraphicalGame::renderCard(const Card& card, Vector2 position) {
        auto tex = this->cardTextures.at(std::make_pair(card.suit, card.face));
        DrawTextureEx(tex, position, 0, CARD_SCALE, WHITE);
    }

    void GraphicalGame::renderCardFaceDown(Vector2 position) {
        DrawTextureEx(this->cardBackTexture, position, 0, CARD_SCALE, WHITE);
    }

    void GraphicalGame::renderStock() {
        if (this->game->hasStock()) {
            this->renderCardFaceDown(this->stockPosition);
        } else {
            DrawTextureEx(this->cardBackTexture, this->stockPosition, 0, CARD_SCALE, TRANSPARENT_CARD_COLOR);
        }
    }

    void GraphicalGame::renderCardPileFaceUp(const CardPile& pile, Vector2 position) {
        for (const auto card : pile) {
            this->renderCard(*card, position);
            position.y += STACKED_DISPLACEMENT;
        }
    }

    void GraphicalGame::renderCardPileFaceDown(std::size_t pileSize, Vector2& position) {
        for (int i = 0; i < pileSize; i++) {
            this->renderCardFaceDown(position);
            position.y += FACE_DOWN_STACKED_DISPLACEMENT;
        }
    }

    void GraphicalGame::render() {
        auto king = this->cardTextures.at(std::make_pair(Suit::SPADES, Face::KING));
        this->renderCardFaceDown(this->stockPosition);
        DrawTexture(king, this->wasteStart.x, this->wasteStart.y, WHITE);
        // TODO empty stock draw transparent card
        // TODO waste
        // TODO foundations

        float x = this->tableauStart.x;
        int deltaX = this->cardBackTexture.width + TINY_SPACING;
        for (int n = 0; n < 7; n++) {
            float y = windowHeight() - this->tableauStart.y;
            auto tableauDrawPos = Vector2 {x, y};
            this->renderCardPileFaceDown(this->game->getClosedTableauSize(n), tableauDrawPos);
            this->renderCardPileFaceUp(this->game->getOpenTableau(n), tableauDrawPos);
            // for (int i = 0; i < n; i++) {
            //     DrawTextureEx(this->cardBackTexture, Vector2 {x, y}, 0, CARD_SCALE, WHITE);
            //     y += FACE_DOWN_STACKED_DISPLACEMENT;
            // }
            // for (int i = 0; i < 13; i++) {
            //     DrawTexture(king, x, y, WHITE);
            //     y += STACKED_DISPLACEMENT;
            // }
            x += deltaX;
        }

        // TODO lifted pile
    }

    int GraphicalGame::cardWidth() {
        return CARD_SCALE * this->cardBackTexture.width;
    }

    int GraphicalGame::cardHeight() {
        return CARD_SCALE * this->cardBackTexture.height;
    }

    void GraphicalGame::detectClick(Vector2 mousePosition) {
        // TODO clickzones
    }

    std::size_t GraphicalGame::windowWidth() {
        return this->actualResolution.x;
    }

    std::size_t GraphicalGame::windowHeight() {
        return this->actualResolution.y;
    }
}