#include "sltgraphics.hpp"

#include <utility>
#include <raymath.h>
#include <iostream>
#include <sstream>

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

        this->calculateBounds();
    }

    GraphicalGame::~GraphicalGame() {
        for (auto entry : this->cardTextures) {
            Texture tex = entry.second;
            UnloadTexture(tex);
        }
        UnloadTexture(this->cardBackTexture);
        delete this->game;
    }

    GraphicalGame::GraphicalGame(std::minstd_rand::result_type seed): GraphicalGame() {
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
        // keep height the same as macro region
        for (int i = 0; i < NUM_TABLEAUS; i++) {
            this->tableauRegions.at(i) = currTableauRegion;
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
            this->foundationRegions.emplace(i, currFoundationRegion);
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
        for (auto card = pile.rbegin(); card != pile.rend(); card++) {
            this->renderCard(**card, position);
            position.y += STACKED_DISPLACEMENT;
        }
    }

    void GraphicalGame::renderCardPileFaceDown(std::size_t pileSize, Vector2& position) {
        for (std::size_t i = 0; i < pileSize; i++) {
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
        for (int i = 0; i < NUM_TABLEAUS; i++) {
            Vector2 currTableauPosition = RectOrigin(this->tableauRegions.at(i));
            this->renderCardTexture(this->cardBackTexture, currTableauPosition, Fade(BLACK, 0.3));
            this->renderCardPileFaceDown(this->game->getClosedTableauSize(i), currTableauPosition);
            this->renderCardPileFaceUp(this->game->getOpenTableau(i), currTableauPosition);
        }
    }

    void GraphicalGame::renderUI() {
        DrawText(TextFormat("Moves: %i", this->game->getMoveCount()), 20, 20, 30, BLACK);
    }

    void GraphicalGame::renderFoundations() {
        for (Suit s = Suit::FIRST; s < Suit::END; s++) {
            Rectangle region = this->foundationRegions.at(s);
            Vector2 position = RectOrigin(region);
            const Card *foundationTop = this->game->peekFoundation(s);
            if (foundationTop == nullptr) {
                Texture ace = this->cardTextures.at(std::make_pair(s, Face::ACE));
                this->renderCardTexture(ace, position, TRANSPARENT_CARD_COLOR);
            } else {
                this->renderCard(*foundationTop, position);
            }
        }
    }

    void GraphicalGame::renderHeldCards() {
        if (this->game->getHeldCards().empty()) {
            return;
        }
        auto drawPos = Vector2Subtract(this->dragPosition, this->dragOffset);
        this->renderCardPileFaceUp(this->game->getHeldCards(), drawPos);
    }

    void GraphicalGame::update() {
        frame++;
    }

    void GraphicalGame::render() {
        this->renderStock();
        this->renderWaste();
        this->renderTableaus();
        this->renderFoundations();
        this->renderHeldCards();
        this->renderUI();
    }

    void GraphicalGame::clickStock() {
        if (this->game->hasStock()) {
            this->game->turnStock();
        } else {
            this->game->returnWasteToStock();
        }
    }

    void GraphicalGame::clickWaste(Vector2 mousePosition) {
        if (!this->game->hasWaste()) {
            return;
        }
        this->game->takeWaste();
        this->dragOffset = Vector2Subtract(mousePosition, RectOrigin(this->wasteRegion));
    }

    void GraphicalGame::clickFoundation(Suit foundationSuit, Vector2 mousePosition) {
        auto foundationRegion = this->foundationRegions.at(foundationSuit);
        if (this->game->hasFoundation(foundationSuit)) {
            this->game->takeFoundation(foundationSuit);
            this->dragOffset = Vector2Subtract(mousePosition, RectOrigin(foundationRegion));
        }
    }

    void GraphicalGame::clickTableau(std::size_t tableauIndex, Vector2 mousePosition) {
        int closedCardsStart = RectOrigin(this->tableauRegions.at(tableauIndex)).y;
        int nClosedCards = this->game->getClosedTableauSize(tableauIndex);
        int nOpenCards = this->game->getOpenTableau(tableauIndex).size();
        if (nOpenCards == 0) { // if card stack needs top card turned over.
            if (this->game->getClosedTableauSize(tableauIndex) == 0) {
                return;
            }
            Rectangle lastClosedCard = this->tableauRegions.at(tableauIndex);
            lastClosedCard.height = this->cardHeight();
            lastClosedCard.y += (nClosedCards - 1) * FACE_DOWN_STACKED_DISPLACEMENT;
            if (CheckCollisionPointRec(mousePosition, lastClosedCard)) {
                this->game->turnClosedTableauTop(tableauIndex);
            }
        } else {
            this->clickStart = this->frame;
            int openCardsStart = closedCardsStart + nClosedCards * FACE_DOWN_STACKED_DISPLACEMENT;
            int nCardsDown = floor((mousePosition.y - openCardsStart) / STACKED_DISPLACEMENT);
            if (nCardsDown < 0) { // if clicking hidden cards under shown tableau.
                return;
            } else if (nCardsDown < nOpenCards) { // if dragging a stack.
                this->game->takeTableau(tableauIndex, nOpenCards - nCardsDown);
                float relativeX = mousePosition.x - this->tableauRegions.at(tableauIndex).x;
                float relativeY = fmod((mousePosition.y - openCardsStart), STACKED_DISPLACEMENT);
                this->dragOffset = { relativeX, relativeY };
            } else { // single card
                Rectangle lastOpenCard = this->tableauRegions.at(tableauIndex);
                lastOpenCard.height = this->cardHeight();
                lastOpenCard.y = openCardsStart + (nOpenCards - 1) * STACKED_DISPLACEMENT;
                if (CheckCollisionPointRec(mousePosition, lastOpenCard)) {
                    this->game->takeTableau(tableauIndex, 1);
                    this->dragOffset = Vector2Subtract(mousePosition, RectOrigin(lastOpenCard));
                }
            }
        }
    }

    void GraphicalGame::handleClick(Vector2 mousePosition) {
        if (CheckCollisionPointRec(mousePosition, this->stockRegion)) {
            this->clickStock();
            return;
        }

        if (CheckCollisionPointRec(mousePosition, this->wasteRegion)) {
            this->clickWaste(mousePosition);
            return;
        }

        if (CheckCollisionPointRec(mousePosition, this->foundationMacroRegion)) {
            for (Suit s = Suit::FIRST; s < Suit::END; s++) {
                auto suitRegion = this->foundationRegions.at(s);
                if (CheckCollisionPointRec(mousePosition, suitRegion)) {
                    this->clickFoundation(s, mousePosition);
                    return;
                }
            }
            return;
        }

        if (CheckCollisionPointRec(mousePosition, this->tableauMacroRegion)) {
            for (int i = 0; i < NUM_TABLEAUS; i++) {
                auto tableauRegion = this->tableauRegions.at(i);
                if (CheckCollisionPointRec(mousePosition, tableauRegion)) {
                    this->clickTableau(i, mousePosition);
                    return;
                }
            }
        }
    }

    void GraphicalGame::handleDrag(Vector2 mousePosition) {
        this->dragPosition = mousePosition;
    }

    void GraphicalGame::releaseDrag(Vector2 mousePosition) {
        if (this->game->getHeldCards().empty()) {
            return;
        }
        for (Suit s = Suit::FIRST; s < Suit::END; s++) {
            float score = this->cardDragOverlapScore(this->foundationRegions.at(s));
            if (score >= CARD_SLOT_MIN_OVERLAP_AREA) {
                try {
                    this->game->stackFoundation(s);
                } catch (const std::exception& e) {
                    std::cerr << "Could not stack " << *this->game->getHeldCards().peekBase();
                    std::cerr << " onto foundation " << s << ": ";
                    std::cerr << e.what() << std::endl;
                    this->cancelDrag();
                }
                return;
            }
        }

        for (std::size_t i = 0; i < NUM_TABLEAUS; i++) {
            float score = this->cardDragOverlapScore(this->tableauRegions.at(i));
            if (score >= CARD_SLOT_MIN_OVERLAP_AREA) {
                try {
                    this->game->stackTableau(i);
                } catch (const std::exception& e) {
                    std::cerr << "Could not stack " << *this->game->getHeldCards().peekBase();
                    std::cerr << " onto tableau " << i << ": ";
                    std::cerr << e.what() << std::endl;
                    this->cancelDrag();
                }
                return;
            }
        }

        this->cancelDrag();
    }

    void GraphicalGame::cancelDrag() {
        this->game->returnHeldCards();
    }

    float GraphicalGame::cardWidth() {
        static float w = CARD_SCALE * this->cardBackTexture.width;
        return w;
    }

    float GraphicalGame::cardHeight() {
        static float h = CARD_SCALE * this->cardBackTexture.height;
        return h;
    }

    float GraphicalGame::cardArea() {
        return this->cardWidth() * this->cardHeight();
    }

    float GraphicalGame::cardDragOverlapScore(Rectangle region) {
        static float maxScore = this->cardArea();
        Vector2 currentCardDragOrigin = Vector2Subtract(this->dragPosition, this->dragOffset);
        Rectangle currentCardDrag = {
            currentCardDragOrigin.x,
            currentCardDragOrigin.y,
            this->cardWidth(),
            this->cardHeight()
        };
        float score = IntersectionArea(currentCardDrag, region);
        float normalizedScore = score / maxScore;
        return normalizedScore;
    }

    std::size_t GraphicalGame::windowWidth() {
        return this->actualResolution.x;
    }

    std::size_t GraphicalGame::windowHeight() {
        return this->actualResolution.y;
    }
}
