#include "card.hpp"
#include "except.hpp"

#include <iostream>

namespace solitaire {
    std::ostream& operator<<(std::ostream& os, const Suit& suit) noexcept {
        static char suitChars[] = {'c', 'd', 'h', 's'};
        return os << suitChars[static_cast<int>(suit)];
    }

    std::ostream& operator<<(std::ostream& os, const Face& face) noexcept {
        // starts at index 1, position 0 is a dummy value
        static char faceChars[] = {'\0', 'A', '2', '3', '4', '5', '6', '7', '8', '9', 'X', 'J', 'Q', 'K'};
        return os << faceChars[static_cast<int>(face)];
    }

    void Card::setFaceUp(bool faceUp) noexcept {
        this->faceUp = faceUp;
    }

    bool Card::isFaceUp() const noexcept {
        return this->faceUp;
    }


    void CardPile::add(Card *c) noexcept {
        this->cards.push_front(c);
    }

    bool CardPile::empty() const noexcept {
        return this->cards.empty();
    }

    std::size_t CardPile::size() const noexcept {
        return this->cards.size();
    }


    const Card *CardPile::peek(std::size_t index) const noexcept {
        if (this->cards.size() > index) {
            return this->cards[index];
        } else {
            return nullptr;
        }
    }

    const Card *CardPile::peekBase() const noexcept {
        return this->peek(this->cards.size() - 1);
    }

    CardPile *CardPile::split(std::size_t amount) {
        auto newPile = new CardPile();

        for (std::size_t i = 0; i < amount; i++) {
            // get the card from top of pile
            Card *currentTop = this->cards[0];

            // remove now-null item from this->cards
            this->cards.pop_front();

            // add it to new pile (in reverse order)
            newPile->cards.push_back(currentTop);
        }

        return newPile;
    }

    void CardPile::stack(CardPile& newTop) noexcept {
        while (!newTop.empty()) {
            auto base = newTop.takeBase();
            this->add(base);
        }
    }

    Card *CardPile::takeTop() {
        if (this->empty()) throw NotEnoughCardsException();

        Card *top = this->cards[0];
        this->cards.pop_front();

        return top;
    }

    Card *CardPile::takeBase() {
        if (this->empty()) throw NotEnoughCardsException();

        Card *base = this->cards.back();
        this->cards.pop_back();

        return base;
    }

    std::ostream& operator<<(std::ostream& os, const Card& card) noexcept {
        return os << card.face << card.suit;
    }
}
