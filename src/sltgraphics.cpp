#include "sltgraphics.hpp"

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
        float longestPossibleTableau = stackPxSize(
            NUM_TABLEAUS - 1,
            static_cast<int>(Face::END),
            this->cardHeight()
        );
        // float stockAndWasteWidth = this->cardWidth() + 2 * SMALL_SPACING;
        float stockY = (this->actualResolution.y - this->cardHeight()) / 2;
        this->stockStart = { TINY_SPACING, stockY };
        this->wasteStart = { this->stockStart.x + this->cardWidth() + TINY_SPACING, stockY };

        float stockAndWasteWidth = this->wasteStart.x + this->cardWidth() + SMALL_SPACING;

        this->tableauStart = { stockAndWasteWidth, longestPossibleTableau };
        // TODO
        this->cardScale = 0.5f;
    }

    void GraphicalGame::render() {
        auto king = this->cardTextures.at(std::make_pair(Suit::SPADES, Face::KING));
        DrawTextureV(this->cardBackTexture, this->stockStart, WHITE);
        DrawTextureV(king, this->wasteStart, WHITE);
        // TODO empty stock draw transparent card
        // TODO waste
        // TODO foundations

        float x = this->tableauStart.x;
        int deltaX = this->cardBackTexture.width + TINY_SPACING;
        for (int n = 0; n < 7; n++) {
            float y = windowHeight() - this->tableauStart.y;
            for (int i = 0; i < n; i++) {
                DrawTexture(this->cardBackTexture, x, y, WHITE);
                y += FACE_DOWN_STACKED_DISPLACEMENT;
            }
            for (int i = 0; i < 13; i++) {
                DrawTexture(king, x, y, WHITE);
                y += STACKED_DISPLACEMENT;
            }
            x += deltaX;
        }

        // TODO lifted pile
    }

    int GraphicalGame::cardWidth() {
        return this->cardScale * this->cardBackTexture.width;
    }

    int GraphicalGame::cardHeight() {
        return this->cardScale * this->cardBackTexture.height;
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