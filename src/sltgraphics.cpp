#include "sltgraphics.hpp"

#include <utility>

#include "utils.hpp"

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

    float getFoundationsWidth(int nSuits, int cardWidth, int spacing) {
        return (nSuits) * cardWidth +
            (nSuits - 1) * spacing;
    }

    void GraphicalGame::calculateBounds() {
        auto const windowPadding = SMALL_SPACING;
        float tallestPossibleTableau = stackPxSize(
            NUM_TABLEAUS - 1,
            static_cast<int>(Face::COUNT),
            this->cardHeight()
        );
        // float stockAndWasteWidth = this->cardWidth() + 2 * SMALL_SPACING;
        float stockY = (this->actualResolution.y - this->cardHeight()) / 2;
        this->stockRegion = {
            windowPadding,
            stockY,
            static_cast<float>(this->cardWidth()),
            static_cast<float>(this->cardHeight()),
        };
        this->wasteRegion = {
            this->stockRegion.x + this->stockRegion.width + TINY_SPACING,
            stockY,
            static_cast<float>(this->cardWidth()),
            static_cast<float>(this->cardHeight())
        };

        auto tableausSpacing = TINY_SPACING;
        auto tableausWidth = NUM_TABLEAUS * this->cardWidth() + (NUM_TABLEAUS - 1) * tableausSpacing;
        this->tableauMacroRegion = Rectangle {
            this->actualResolution.x - windowPadding - tableausWidth,
            this->actualResolution.y - tallestPossibleTableau,
            tableausWidth,
            tallestPossibleTableau
        };

        Rectangle currTableauRegion = this->tableauMacroRegion;
        currTableauRegion.width = this->cardWidth();
        currTableauRegion.height = this->cardHeight();
        for (int i = 0; i < NUM_TABLEAUS; i++) {
            this->tableauRegions[i] = currTableauRegion;
            currTableauRegion.x += currTableauRegion.width + tableausSpacing;
        }


        auto foundationsSpacing = TINY_SPACING;
        auto foundationSize = Vector2 {
            getFoundationsWidth(static_cast<int>(Suit::END), this->cardWidth(), foundationsSpacing),
            static_cast<float>(this->cardHeight()),
        };
        auto foundationTopStripe = Rectangle {
            0.0f,
            0.0f,
            this->actualResolution.x,
            this->tableauMacroRegion.y
        };
        Vector2 foundationOrigin = Center(foundationSize, foundationTopStripe);
        this->foundationMacroRegion = {
            foundationOrigin.x,
            foundationOrigin.y,
            foundationSize.x,
            foundationSize.y
        };
        Rectangle currFoundationRegion = this->foundationMacroRegion;
        currFoundationRegion.width = this->cardWidth();
        currFoundationRegion.height = this->cardHeight();
        for (Suit i = Suit::FIRST; i < Suit::END; i++) {
            this->foundationRegions[static_cast<int>(i)] = currFoundationRegion;
            currFoundationRegion.x += currFoundationRegion.width + foundationsSpacing;
        }
    }

    void GraphicalGame::renderCard(const Card& card, Vector2 position) {
        auto tex = this->cardTextures.at(std::make_pair(card.suit, card.face));
        DrawTextureEx(tex, position, 0, CARD_SCALE, WHITE);
    }

    void GraphicalGame::renderCardFaceDown(Vector2 position) {
        DrawTextureEx(this->cardBackTexture, position, 0, CARD_SCALE, WHITE);
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

    void GraphicalGame::renderStock() {
        auto pos = RectOrigin(this->stockRegion);
        if (this->game->hasStock()) {
            this->renderCardFaceDown(pos);
        } else {
            DrawTextureEx(this->cardBackTexture, pos, 0, CARD_SCALE, TRANSPARENT_CARD_COLOR);
        }
    }

    void GraphicalGame::renderWaste() {
        auto wasteTop = this->game->peekWaste();
        if (wasteTop != nullptr) {
            this->renderCard(*wasteTop, RectOrigin(this->wasteRegion));
        }
    }

    void GraphicalGame::renderFoundation() {

    }

    void GraphicalGame::render() {
        this->renderStock();
        this->renderWaste();
        // this->renderCardFaceDown(this->stockRegion);
        auto king = this->cardTextures.at(std::make_pair(Suit::SPADES, Face::KING));
        // DrawTexture(king, this->wastePosition.x, this->wastePosition.y, WHITE);

        float x = this->tableauMacroRegion.x;
        int deltaX = this->cardBackTexture.width + TINY_SPACING;
        for (int n = 0; n < 7; n++) {
            float y = this->tableauMacroRegion.y;
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
        // TODO foundations

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