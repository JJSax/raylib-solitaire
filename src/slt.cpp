#include "slt.hpp"
#include "except.hpp"

#include <sstream>

namespace solitaire {
    bool suitsCanAlternate(Suit s1, Suit s2) noexcept;
    void throwIfCantStackInTableau(const CardPile& cardPile, const Card& newCard);
    void throwIfCantStackInFoundation(Suit foundationSuit, const CardPile& pile, const Card& newCard);

    bool suitsCanAlternate(Suit s1, Suit s2) noexcept {
        // works due to the ordering of the suits: endpoints add
        // to 3 and are both black, midpoints add to 3 and are both red.
        // any other pair will not add up to three, therefore we have
        // this function
        return static_cast<int>(s1) + static_cast<int>(s2) != 3;
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
    }

    void Game::turnClosedTableauTop(std::size_t index) {
        if (!this->openTableau.at(index).empty()) {
            throw InvalidStateException("Cannot flip closed tableau while there are cards above it in the open tableau.");
        }
        if (this->closedTableau.at(index).empty()) {
            throw NotEnoughCardsException();
        }
        auto card = this->closedTableau.at(index).takeTop();
        this->openTableau.at(index).add(card);
    }

    void Game::returnWasteToStock() {
        if (this->hasStock()) {
            throw InvalidStateException("Cannot turn waste onto stock if stock is not empty.");
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
            throw InvalidStateException("No cards are currently being held.");
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
            throw InvalidStateException("Cannot hold cards while cards are already being held.");
        }
    }

    void Game::takeWaste() {
        this->throwIfAttemptingToHoldMoreCards();
        CardPile *topCard = this->waste.split(1);
        this->heldCards.stack(*topCard);
        this->heldCardsSource = PossibleHeldCardsSource::WASTE;
        delete topCard;
    }

    void Game::takeFoundation(Suit s) {
        this->throwIfAttemptingToHoldMoreCards();
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

    void Game::stackTableau(std::size_t index, CardPile& cards) {
        const Card *topBase = cards.peekBase();
        if (topBase == nullptr) {
            throw NotEnoughCardsException();
        }

        throwIfCantStackInTableau(this->openTableau.at(index), *topBase);
        this->openTableau.at(index).stack(cards);
    }

    void Game::stackFoundation(Suit suit, CardPile& cards) {
        if (cards.empty()) {
            throw NotEnoughCardsException();
        } else if (cards.size() > 1) {
            throw TooManyCardsException();
        }
        const Card *single = cards.peek();

        throwIfCantStackInFoundation(single->suit, this->foundation.at(single->suit), *single);

        this->foundation.at(single->suit).stack(cards);
    }


    void Game::deal(CardPile& onto) {
        auto newCard = this->stock.takeTop();
        onto.add(newCard);
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
        for (int i = 0; i < this->closedTableau.size(); i++) {
            for (int j = 0; j < i; j++) {
                this->deal(this->closedTableau.at(i));
            }
        }
    }

    void Game::dealOpenTableau() {
        for (int i = 0; i < this->openTableau.size(); i++) {
            this->deal(this->openTableau.at(i));
        }
    }
}