#include "game.hpp"
#include "except.hpp"

namespace solitaire {
    bool suitsCanAlternate(Suit s1, Suit s2) noexcept {
        // works due to the ordering of the suits: endpoints add
        // to 3 and are both black, midpoints add to 3 and are both red.
        // any other pair will not add up to three, therefore we have
        // this function
        return s1 + s2 != 3;
    }

    bool canStackInTableau(const CardPile& pile, const Card& newCard) noexcept {
        if (pile.empty()) {
            return newCard.face == Face::KING;
        }
        const Card *oldTop = pile.peek();
        if (!suitsCanAlternate(oldTop->suit, newCard.suit)) {
            return false;
        }
        return oldTop->face == newCard.face + 1;
    }

    bool canStackInFoundation(Suit foundationSuit, const CardPile& pile, const Card& newCard) noexcept {
        if (newCard.suit != foundationSuit) {
            return false;
        }
        if (pile.empty()) {
            return newCard.face == Face::ACE;
        }
        const Card *oldTop = pile.peek();
        return oldTop->face == newCard.face - 1;
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

    const Card *Game::peekWaste() const noexcept {
        return this->waste.peek();
    }

    Card *Game::takeWaste() {
        return this->waste.takeTop();
    }

    const CardPile& Game::getOpenTableau(std::size_t index) const {
        return this->openTableau[index];
    }

    std::size_t Game::getClosedTableauSize(std::size_t index) const {
        return this->closedTableau[index].size();
    }

    bool Game::stackTableau(std::size_t index, CardPile& cards) {
        auto topBase = cards.peekBase();
        if (topBase == nullptr) {
            return false;
        }

        if (!canStackInTableau(this->openTableau[index], *topBase)) {
            return false;
        }

        this->openTableau[index].stack(cards);
        return true;
    }


    void Game::deal(CardPile& onto) {
        auto newCard = this->stock.takeTop();
        onto.add(newCard);
    }

    void Game::initFullDeckInOrder() noexcept {
        for (int s = SUITS_FIRST; s < SUITS_END; s++) {
            Suit mySuit = static_cast<Suit>(s);
            for (int f = FACES_FIRST; f < FACES_END; f++) {
                Card *c = new Card(static_cast<Face>(f), static_cast<Suit>(s));
                this->allCards.push_back(c);
                this->stock.add(c);
            }
        }
    }

    void Game::initFoundations() noexcept {
        for (int i = CLUBS; i <= SPADES; i++) {
            Suit s = static_cast<Suit>(i);
            this->foundation.emplace(s, CardPile());
        }
    }

    void Game::dealClosedTableau() {
        for (int i = 0; i < this->closedTableau.size(); i++) {
            for (int j = 0; j < i; j++) {
                this->deal(this->closedTableau[i]);
            }
        }
    }

    void Game::dealOpenTableau() {
        for (int i = 0; i < this->openTableau.size(); i++) {
            this->deal(this->openTableau[i]);
        }
    }
}