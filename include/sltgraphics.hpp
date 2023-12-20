#pragma once

#include "slt.hpp"
#include "sltconfig.hpp"

#include <raylib.h>

#include <memory>
#include <sstream>

namespace solitaire {
    class GraphicalGame {
        GraphicalGame();

        Game *game;
        float windowScale = 1.0f;
        float cardScale = 1.0f;

        CardPile *heldCards;

        std::map<std::pair<Suit, Face>, Texture> cardTextures;
        Texture cardBackTexture;

        void renderCard(const Card& card, Vector2 position);
        void renderCardPile(const CardPile& pile, Vector2 position);

    public:
        GraphicalGame(std::minstd_rand::result_type seed);

        ~GraphicalGame();

        template<typename URNG>
        static GraphicalGame *create(URNG& rand) {
            GraphicalGame *ggame = new GraphicalGame();
            ggame->game = Game::createAndDealGame(rand);
            return ggame;
        }

        void render();
        void detectClick(Vector2 mousePosition);

        std::size_t windowWidth();
        std::size_t windowHeight();
    };
}