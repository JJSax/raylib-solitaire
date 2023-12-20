#include "card.hpp"
#include "except.hpp"

#include <iostream>

namespace solitaire {
    char suitToChar(Suit s) {
        static char suitChars[] = {'c', 'd', 'h', 's', '\0'};
        return  suitChars[static_cast<int>(s)];
    }

    Suit& operator++(Suit& s) {
        s = static_cast<Suit>(static_cast<int>(s) + 1);
        if (s > Suit::END) s = Suit::FIRST;
        return s;
    }

    Suit operator++(Suit& s, int) {
        Suit result(s);
        ++s;
        return result;
    }

    std::ostream& operator<<(std::ostream& os, const Suit& suit) noexcept {
        return os << suitToChar(suit);
    }


    char faceToChar(Face f) {
        static char faceChars[] = {'\0', 'A', '2', '3', '4', '5', '6', '7', '8', '9', 'X', 'J', 'Q', 'K', '\0'};
        return faceChars[static_cast<int>(f)];
    }

    Face& operator++(Face& f) {
        f = static_cast<Face>(static_cast<int>(f) + 1);
        if (f > Face::END) f = Face::FIRST;
        return f;
    }

    Face operator++(Face& f, int) {
        Face result(f);
        ++f;
        return result;
    }

    std::ostream& operator<<(std::ostream& os, const Face& face) noexcept {
        // starts at index 1, position 0 is a dummy value
        return os << faceToChar(face);
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
        if (this->cards.size() > 0) {
            return this->cards.back();
        } else {
            return nullptr;
        }
    }

    CardPile *CardPile::split(std::size_t amount) {
        auto newPile = new CardPile();
        if (amount > this->size()) throw NotEnoughCardsException();

        for (std::size_t i = 0; i < amount; i++) {
            Card *currentTop = this->cards[0];
            this->cards.pop_front();
            newPile->cards.push_back(currentTop);
        }

        return newPile;
    }

    void CardPile::stack(CardPile& newTop) noexcept {
        while (!newTop.empty()) {
            Card *base = newTop.takeBase();
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
