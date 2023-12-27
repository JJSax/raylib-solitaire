#pragma once

#include "slt.hpp"
#include "sltconfig.hpp"

#include <raylib.h>

#include <memory>
#include <sstream>

namespace solitaire {
    class GraphicalGame {
        GraphicalGame();

        void renderCardTexture(const Texture& texture, Vector2 position, Color=WHITE);
        void renderCard(const Card& card, Vector2 position);
        void renderCardFaceDown(Vector2 position);
        void renderCardPileFaceUp(const CardPile& pile, Vector2 position);
        void renderCardPileFaceDown(std::size_t pileSize, Vector2& position);

        void renderStock();
        void renderWaste();
        void renderTableaus();
        void renderFoundations();
        void renderHeldCards();

        float cardWidth();
        float cardHeight();
        float cardArea();

        float cardDragOverlapScore(Rectangle region);

        void clickStock();
        void clickWaste(Vector2 mousePosition);
        void clickFoundation(Suit which, Vector2 mousePosition);
        void clickTableau(std::size_t which, Vector2 mousePosition);

        void cancelDrag();

        Rectangle stockRegion;
        Rectangle wasteRegion;
        Rectangle tableauMacroRegion;
        std::array<Rectangle, NUM_TABLEAUS> tableauRegions;
        Rectangle foundationMacroRegion;
        std::unordered_map<Suit, Rectangle> foundationRegions;

        Game *game;
        Vector2 actualResolution;
        float cardScale = 1.0f;

        Vector2 dragPosition;
        Vector2 dragOffset;

        std::map<std::pair<Suit, Face>, Texture> cardTextures;
        Texture cardBackTexture;

    public:
        GraphicalGame(std::minstd_rand::result_type seed);

        ~GraphicalGame();

        /**
         * @brief Calculate region
         *
         */
        void calculateBounds();

        /**
         * @brief Creates and initializes a game with the given PRNG.
         *
         * @tparam URNG The uniform PRNG type to shuffle the cards with.
         * @param rand The uniform PRNG instance to use.
         * @return std::unique_ptr<GraphicalGame>
         */
        template<typename URNG>
        static std::unique_ptr<GraphicalGame> create(URNG& rand) {
            GraphicalGame *ggame = new GraphicalGame();
            ggame->game = Game::createAndDealGame(rand);
            return std::unique_ptr<GraphicalGame>(ggame);
        }

        /// @brief Renders the game.
        void render();

        /**
         * @brief Signals a new click and possibly the start of a drag.
         * @param mousePosition Position of the click on the window.
         */
        void handleClick(Vector2 mousePosition);

        /**
         * @brief Updates mouse position while dragging
         * @param mousePosition New mouse position while dragging.
         */
        void handleDrag(Vector2 mousePosition);

        /**
         * @brief Releases currently held cards at the given mouse position.
         * @param mousePosition Position of the mouse when the mouse button was released.
         */
        void releaseDrag(Vector2 mousePosition);

        /**
         * @brief Gets the width of the window.
         * @return std::size_t The width of the window.
         */
        std::size_t windowWidth();

        /**
         * @brief Gets the height of the window.
         * @return std::size_t The height of the window.
         */
        std::size_t windowHeight();
    };
}