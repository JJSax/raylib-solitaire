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

    void GraphicalGame::render() {
        // TODO stock
        // TODO waste
        // TODO foundations
        // TODO closed tableaus
        // TODO open tableaus

        // TODO lifted pile
        DrawTexture(this->cardBackTexture, 100, 100, WHITE);
    }

    void GraphicalGame::detectClick(Vector2 mousePosition) {
        // TODO clickzones
    }

    std::size_t GraphicalGame::windowWidth() {
        return this->windowScale * TARGET_RESOLUTION.x;
    }

    std::size_t GraphicalGame::windowHeight() {
        return this->windowScale * TARGET_RESOLUTION.y;
    }
}