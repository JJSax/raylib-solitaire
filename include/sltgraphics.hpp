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

        void clickStock();
        void clickWaste(Vector2 mousePosition);
        void clickFoundation(Suit which, Vector2 mousePosition);
        void clickTableau(std::size_t which, Vector2 mousePosition);
        // void clickTableau(std::size_t which, std::size_t stackHeight);

        void cancelDrag();

        Rectangle stockRegion;
        Rectangle wasteRegion;
        Rectangle tableauMacroRegion;
        std::array<Rectangle, static_cast<int>(Face::COUNT)> tableauRegions;
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

        void calculateBounds();

        template<typename URNG>
        static std::unique_ptr<GraphicalGame> create(URNG& rand) {
            GraphicalGame *ggame = new GraphicalGame();
            ggame->game = Game::createAndDealGame(rand);
            return std::unique_ptr<GraphicalGame>(ggame);
        }

        void render();
        void handleClick(Vector2 mousePosition);
        void handleDrag(Vector2 mousePosition);
        void releaseDrag(Vector2 mousePosition);

        std::size_t windowWidth();
        std::size_t windowHeight();

        friend std::unique_ptr<GraphicalGame> debugScript(std::unique_ptr<GraphicalGame> game);
    };
}