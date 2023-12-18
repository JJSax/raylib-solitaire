#include "game.hpp"
#include "except.hpp"

#include <sstream>

namespace solitaire {
    bool suitsCanAlternate(Suit s1, Suit s2) noexcept {
        // works due to the ordering of the suits: endpoints add
        // to 3 and are both black, midpoints add to 3 and are both red.
        // any other pair will not add up to three, therefore we have
        // this function
        return static_cast<int>(s1) + static_cast<int>(s2) != 3;
    }

    bool canStackInTableau(const CardPile& pile, const Card& newCard) noexcept {
        if (pile.empty()) {
            return newCard.face == Face::KING;
        }
        const Card *oldTop = pile.peek();
        if (!suitsCanAlternate(oldTop->suit, newCard.suit)) {
            return false;
        }
        Face newCardFace = newCard.face;

        // I'm too lazy to implement operator+.
        return oldTop->face == (++newCardFace);
    }

    bool canStackInFoundation(Suit foundationSuit, const CardPile& pile, const Card& newCard) noexcept {
        if (newCard.suit != foundationSuit) {
            return false;
        }
        if (pile.empty()) {
            return newCard.face == Face::ACE;
        }
        Face oldTopFace = pile.peek()->face;

        // I'm too lazy to implement operator+.
        return (++oldTopFace) == newCard.face;
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

    std::unique_ptr<CardPile> Game::splitTableau(std::size_t index, std::size_t amount) {
        throwIfTableauIndexOutOfRange(index);
        CardPile *splitOff = this->openTableau[index].split(amount);
        return std::unique_ptr<CardPile>(splitOff);
    }

    bool Game::stackTableau(std::size_t index, CardPile& cards) noexcept {
        throwIfTableauIndexOutOfRange(index);
        const Card *topBase = cards.peekBase();
        if (topBase == nullptr) {
            return false;
        }

        if (!canStackInTableau(this->openTableau[index], *topBase)) {
            return false;
        }

        this->openTableau[index].stack(cards);
        return true;
    }

    bool Game::stackFoundation(Suit suit, CardPile& cards) noexcept {
        if (cards.size() != 1) {
            return false;
        }
        const Card *single = cards.peek();

        if (!canStackInFoundation(single->suit, this->foundation[single->suit], *single)) {
            return false;
        }

        this->foundation[single->suit].stack(cards);
        return true;
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
                this->deal(this->closedTableau[i]);
            }
        }
    }

    void Game::dealOpenTableau() {
        for (int i = 0; i < this->openTableau.size(); i++) {
            this->deal(this->openTableau[i]);
        }
    }

    void Game::throwIfTableauIndexOutOfRange(std::size_t index) {
        if (index >= Game::NUM_TABLEAUS) {
            std::stringstream message("tableau at index ");
            message << index << " does not exist";
            throw std::out_of_range(message.str());
        }
    }
}