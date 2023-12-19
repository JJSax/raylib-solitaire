#pragma once

#include "slt.hpp"
#include "sltconfig.hpp"

extern "C" {
#include <raylib.h>
}

#include <memory>

namespace solitaire {
    class GraphicalGame {
        Game *game;

        GraphicalGame() {}

        ~GraphicalGame() {
            delete game;
        }

    public:
        // TODO load in card images
        template<typename URNG>
        static GraphicalGame *createAndDealGame(URNG& rand) {
            GraphicalGame *ggame = new GraphicalGame();
            ggame->game = Game::createAndDealGame(rand);
            return ggame;
        }

        GraphicalGame(std::minstd_rand::result_type seed=0) {
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

        }
    };
}