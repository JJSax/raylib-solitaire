#pragma once

#include "slt.hpp"
#include "sltconfig.hpp"

extern "C" {
#include <raylib.h>
}

#include <memory>
#include <sstream>

namespace solitaire {
    class GraphicalGame {
        Game *game;
        float windowScale = 1.0f;
        float cardScale = 1.0f;

        CardPile *heldCards;

        std::map<std::pair<Suit, Face>, Texture> cardTextures;

        GraphicalGame() {
            std::string basePath(CARD_TEXTURE_PATH_PREFIX);
            for (Suit s = Suit::FIRST; s < Suit::END; s++) {
                std::stringstream suitPath(basePath);
                suitPath << '/' << suitToChar(s);
                for (Face f = Face::FIRST; f < Face::END; f++) {
                    std::stringstream cardPath(suitPath.str());
                    cardPath << '/' << faceToChar(f) << ".png";

                    Texture tex = LoadTexture(cardPath.str().c_str());
                    this->cardTextures.emplace(std::make_pair(s, f), tex);
                }
            }
        }

        void drawCard(const Card& card, Vector2 position);
        void drawCardPile(const CardPile& pile, Vector2 position);

    public:

        ~GraphicalGame() {
            for (auto entry : this->cardTextures) {
                Texture tex = entry.second;
                UnloadTexture(tex);
            }
            delete this->game;
        }

        template<typename URNG>
        static GraphicalGame *createAndDealGame(URNG& rand) {
            GraphicalGame *ggame = new GraphicalGame();
            ggame->game = Game::createAndDealGame(rand);
            return ggame;
        }

        GraphicalGame(std::minstd_rand::result_type seed) {
            auto rand = std::minstd_rand(seed);
            this->game = Game::createAndDealGame<std::minstd_rand>(rand);
        }

        void draw() {
            // TODO stock
            // TODO waste
            // TODO foundations
            // TODO closed tableaus
            // TODO open tableaus

            // TODO lifted pile
        }

        void detectClick(Vector2 mousePosition) {
            // TODO clickzones
        }

        std::size_t windowWidth() {
            return this->windowScale * TARGET_RESOLUTION.x;
        }

        std::size_t windowHeight() {
            return this->windowScale * TARGET_RESOLUTION.y;
        }
    };
}