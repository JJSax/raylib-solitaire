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
            this->tableauMacroRegion.x,
            0.0f,
            this->tableauMacroRegion.width,
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

    void GraphicalGame::renderCardTexture(const Texture& texture, Vector2 position, Color color) {
        DrawTextureEx(texture, position, 0, CARD_SCALE, color);
    }

    void GraphicalGame::renderCard(const Card& card, Vector2 position) {
        auto tex = this->cardTextures.at(std::make_pair(card.suit, card.face));
        this->renderCardTexture(tex, position);
    }

    void GraphicalGame::renderCardFaceDown(Vector2 position) {
        this->renderCardTexture(this->cardBackTexture, position);
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
            this->renderCardTexture(this->cardBackTexture, pos, TRANSPARENT_CARD_COLOR);
        }
    }

    void GraphicalGame::renderWaste() {
        auto wasteTop = this->game->peekWaste();
        if (wasteTop != nullptr) {
            this->renderCard(*wasteTop, RectOrigin(this->wasteRegion));
        }
    }

    void GraphicalGame::renderTableaus() {
        auto tableauDrawPos = RectOrigin(this->tableauMacroRegion);
        for (int i = 0; i < NUM_TABLEAUS; i++) {
            Vector2 currTableauPosition = RectOrigin(this->tableauRegions[i]);
            this->renderCardPileFaceDown(this->game->getClosedTableauSize(i), currTableauPosition);
            this->renderCardPileFaceUp(this->game->getOpenTableau(i), currTableauPosition);
            // for (int i = 0; i < n; i++) {
            //     DrawTextureEx(this->cardBackTexture, Vector2 {x, y}, 0, CARD_SCALE, WHITE);
            //     y += FACE_DOWN_STACKED_DISPLACEMENT;
            // }
            // for (int i = 0; i < 13; i++) {
            //     DrawTexture(king, x, y, WHITE);
            //     y += STACKED_DISPLACEMENT;
            // }
        }
    }

    void GraphicalGame::renderFoundations() {
        for (Suit s = Suit::FIRST; s < Suit::END; s++) {
            Rectangle region = this->foundationRegions[static_cast<int>(s)];
            Vector2 position = RectOrigin(region);
            const Card *foundationTop = this->game->peekFoundation(s);
            if (foundationTop == nullptr) {
                Texture ace = this->cardTextures.at(std::make_pair(s, Face::ACE));
                this->renderCardTexture(ace, position, TRANSPARENT_CARD_COLOR);
            } else {

            }
        }
    }

    void GraphicalGame::render() {
        this->renderStock();
        this->renderWaste();
        this->renderTableaus();
        this->renderFoundations();

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