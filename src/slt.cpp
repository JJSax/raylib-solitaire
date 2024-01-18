#include "slt.hpp"
#include "except.hpp"
#include "options.hpp"

#include <sstream>
    #include <iostream>

namespace solitaire {
    bool suitsCanAlternate(Suit s1, Suit s2) noexcept {
        // works due to the ordering of the suits: endpoints add
        // to 3 and are both black, midpoints add to 3 and are both red.
        // any other pair will not add up to three, therefore we have
        // this function
        // same suits add to 3, so test for that included.
        int f = static_cast<int>(s1);
        int s = static_cast<int>(s2);
        return f + s != 3 && f != s;
    }

    bool Game::canStack(const CardPile& bottom, const CardPile& top) {
        if (bottom.empty()) {
            return top.peekBase()->face == Face::KING;
        }

        const Card *bottomContact = bottom.peek();
        const Card *topContact = top.peekBase();
        Face tcFace = topContact->face;
        ++tcFace;

        return suitsCanAlternate(bottomContact->suit, topContact->suit) && tcFace == bottomContact->face;
    }

    void throwIfCantStackInTableau(const CardPile& pile, const Card& newCard) {
        if (pile.empty()) {
            if (newCard.face == Face::KING) {
                return;
            } else {
                throw InvalidCardPlacementException();
            }
        }
        const Card *oldTop = pile.peek();
        if (!suitsCanAlternate(oldTop->suit, newCard.suit)) {
            throw MismatchedSuitsException();
        }
        Face newCardFace = newCard.face;

        // I'm too lazy to implement operator+.
        if (oldTop->face != (++newCardFace)) {
            throw NonSequentialFacesException();
        }
    }

    void throwIfCantStackInFoundation(Suit foundationSuit, const CardPile& pile, const Card& newCard) {
        if (newCard.suit != foundationSuit) {
            throw MismatchedSuitsException();
        }
        if (pile.empty()) {
            if (newCard.face == Face::ACE) {
                return; // ok
            } else {
                throw InvalidCardPlacementException();
            }
        }
        Face oldTopFace = pile.peek()->face;

        // I'm too lazy to implement operator+.
        if ((++oldTopFace) != newCard.face) {
            throw NonSequentialFacesException();
        }
    }

    Game::Game() {
        this->initFullDeckInOrder();
    }

    Game::~Game() {
        for (Card *c : this->allCards) {
            delete c;
        }
        this->allCards.clear();
    }

    void Game::dealGame() {
        this->moves = 0;
        this->initFoundations();
        this->dealClosedTableau();
        this->dealOpenTableau();
    }

    bool Game::hasStock() const noexcept {
        return !this->stock.empty();
    }

    void Game::turnStock() {
        if (this->stock.empty()) {
            throw NotEnoughCardsException();
        }
        auto card = this->stock.takeTop();
        this->waste.add(card);
        this->moves++;
    }

    void Game::turnClosedTableauTop(std::size_t index) {
        if (!this->openTableau.at(index).empty()) {
            throw std::logic_error("Cannot flip closed tableau while there are cards above it in the open tableau.");
        }
        if (this->closedTableau.at(index).empty()) {
            throw NotEnoughCardsException();
        }
        auto card = this->closedTableau.at(index).takeTop();
        this->openTableau.at(index).add(card);
    }

    void Game::returnWasteToStock() {
        if (this->hasStock()) {
            throw std::logic_error("Cannot turn waste onto stock if stock is not empty.");
        }
        this->waste.turnOnto(this->stock);
    }

    const Card *Game::peekWaste() const noexcept {
        return this->waste.peek();
    }

    const Card *Game::peekFoundation(Suit s) const {
        return this->foundation.at(s).peek();
    }

    void Game::returnHeldCards() {
        if (this->heldCards.empty()) {
            throw std::logic_error("No cards are currently being held.");
        }
        switch (this->heldCardsSource) {
            Suit s;
            std::size_t index;
            case PossibleHeldCardsSource::WASTE:
                this->waste.stack(this->heldCards);
                break;
            case PossibleHeldCardsSource::FOUNDATION:
                s = this->heldSourcePileExtra.foundationSuit;
                this->foundation.at(s).stack(this->heldCards);
                break;
            case PossibleHeldCardsSource::TABLEAU:
                index = this->heldSourcePileExtra.tableauIndex;
                this->openTableau.at(index).stack(this->heldCards);
                break;
        }
    }

    const CardPile& Game::getHeldCards() {
        return this->heldCards;
    }

    void Game::throwIfAttemptingToHoldMoreCards() {
        if (!this->heldCards.empty()) {
            throw std::logic_error("Cannot hold cards while cards are already being held.");
        }
    }

    void Game::throwIfAttemptingToGrabEmptyPile(CardPile pile) {
        if (this->heldCards.empty() && pile.empty()) {
            throw std::logic_error("Cannot grab an empty pile.");
        }
    }

    void Game::takeWaste() {
        this->throwIfAttemptingToHoldMoreCards();
        CardPile *topCard = this->waste.split(1);
        this->heldCards.stack(*topCard);
        this->heldCardsSource = PossibleHeldCardsSource::WASTE;
        delete topCard;
    }

    bool Game::hasWaste() {
        return !this->waste.empty();
    }

    void Game::takeFoundation(Suit s) {
        this->throwIfAttemptingToHoldMoreCards();
        this->throwIfAttemptingToGrabEmptyPile(this->foundation.at(s));

        CardPile *topCard = this->foundation.at(s).split(1);
        this->heldCards.stack(*topCard);
        this->heldCardsSource = PossibleHeldCardsSource::FOUNDATION;
        this->heldSourcePileExtra.foundationSuit = s;
        delete topCard;
    }

    void Game::takeTableau(std::size_t index, std::size_t amount) {
        this->throwIfAttemptingToHoldMoreCards();
        CardPile *split = this->openTableau.at(index).split(amount);
        this->heldCards.stack(*split);
        this->heldCardsSource = PossibleHeldCardsSource::TABLEAU;
        this->heldSourcePileExtra.tableauIndex = index;
        delete split;
    }

    const CardPile& Game::getOpenTableau(std::size_t index) const {
        return this->openTableau.at(index);
    }

    std::size_t Game::getClosedTableauSize(std::size_t index) const {
        return this->closedTableau.at(index).size();
    }

    void Game::stackTableau(std::size_t index) {
        if (this->heldCards.empty()) {
            throw NotEnoughCardsException();
        }

        throwIfCantStackInTableau(this->openTableau.at(index), *this->heldCards.peekBase());
        if (index != this->heldSourcePileExtra.tableauIndex) {
            this->moves++;
        }

        int heldIndex = this->heldSourcePileExtra.tableauIndex;
        this->openTableau.at(index).stack(this->heldCards);
        if (config::autoplayClosedTableauTop
            && this->getClosedTableauSize(heldIndex) > 0
            && this->getOpenTableau(heldIndex).size() == 0
            && this->heldCardsSource == PossibleHeldCardsSource::TABLEAU
        ) {
            this->turnClosedTableauTop(heldIndex);
        }
    }

    void Game::stackFoundation(Suit suit) {
        if (this->heldCards.empty()) {
            throw NotEnoughCardsException();
        } else if (this->heldCards.size() > 1) {
            throw TooManyCardsException();
        }
        const Card *single = this->heldCards.peek();

        throwIfCantStackInFoundation(suit, this->foundation.at(suit), *single);

        int heldIndex = this->heldSourcePileExtra.tableauIndex;
        this->foundation.at(suit).stack(this->heldCards);
        if (
            config::autoplayClosedTableauTop
            && this->heldCardsSource == PossibleHeldCardsSource::TABLEAU
            && this->getClosedTableauSize(heldIndex) > 0
            && this->getOpenTableau(heldIndex).size() == 0
        ) {
            this->turnClosedTableauTop(heldIndex);
        }

        if (this->heldCardsSource != PossibleHeldCardsSource::FOUNDATION) {
            this->moves++;
        }
    }

    bool Game::hasFoundation(Suit suit) {
        return !this->foundation.at(suit).empty();
    }

    void Game::deal(CardPile& onto) {
        auto newCard = this->stock.takeTop();
        onto.add(newCard);
    }

    int Game::getMoveCount() { return moves; }

    void Game::attemptHeldToFoundation() {
        if (this->heldCards.size() != 1) return;

        Card heldCard = *this->heldCards.peek();
        Face heldFace = this->heldCards.peekBase()->face;
        for (Suit s = Suit::FIRST; s < Suit::END; s++) {
            if (heldCard.suit == s && heldCard.face == Face::ACE) {
                this->stackFoundation(s);
                return;
            }
            if (!this->hasFoundation(s)) continue; // prevent checking a foundation that doesn't exist.

            Face topFace = this->foundation.at(s).peek()->face;
            if (
                this->heldCards.peekBase()->suit == s
                && (++topFace) == heldFace
            ) {
                this->stackFoundation(s);
                return;
            }
        }

    }

    void Game::attemptHeldToTableau() {
        Face heldContact = this->heldCards.peekBase()->face;

        std::vector<std::size_t> valid;

        for (std::size_t i = 0; i < NUM_TABLEAUS; i++) {
            if (this->heldCardsSource == PossibleHeldCardsSource::TABLEAU
                && this->heldSourcePileExtra.tableauIndex == i
            ) continue;

            if (heldContact == Face::KING && this->getOpenTableau(i).empty()) {
                this->stackTableau(i);
                return;
            }

            if (!this->getOpenTableau(i).empty() && this->canStack(this->getOpenTableau(i), this->heldCards)) {
                valid.emplace_back(i);
            }
        }

        if (!valid.empty()) {
            this->stackTableau(valid[0]);
        }
    }

    void Game::initFullDeckInOrder() noexcept {
        for (Suit s = Suit::FIRST; s < Suit::END; s++) {
            for (Face f = Face::FIRST; f < Face::END; f++) {
                Card *c = new Card(f, s);
                this->allCards.push_back(c);
                this->stock.add(c);
            }
        }
    }

    void Game::initFoundations() noexcept {
        for (Suit s = Suit::FIRST; s < Suit::END; s++) {
            this->foundation.emplace(s, CardPile());
        }
    }

    void Game::dealClosedTableau() {
        for (std::size_t i = 0; i < this->closedTableau.size(); i++) {
            for (std::size_t j = 0; j < i; j++) {
                this->deal(this->closedTableau.at(i));
            }
        }
    }

    void Game::dealOpenTableau() {
        for (std::size_t i = 0; i < this->openTableau.size(); i++) {
            this->deal(this->openTableau.at(i));
        }
    }
}
