#include "card.hpp"

#include <iostream>

namespace solitaire {

    std::ostream& operator<<(std::ostream& os, const Suit& suit) {
        static char suitChars[] = {'c', 'd', 'h', 's'};
        return os << suitChars[static_cast<int>(suit)];
    }

    std::ostream& operator<<(std::ostream& os, const Face& face) {
        // starts at index 1, position 0 is a dummy value
        static char faceChars[] = {'\0', 'A', '2', '3', '4', '5', '6', '7', '8', '9', 'X', 'J', 'Q', 'K'};
        return os << faceChars[static_cast<int>(face)];
    }

    void Card::setFaceUp(bool faceUp) {
        this->faceUp = faceUp;
    }

    bool Card::isFaceUp() {
        return this->faceUp;
    }


    void CardPile::add(const Card& c) {
        // top of the pile is index 0
        this->cards.push_front(std::make_shared<Card>(c));
    }

    bool CardPile::empty() {
        return this->cards.empty();
    }

    std::size_t CardPile::size() {
        return this->cards.size();
    }


    std::optional<const std::weak_ptr<Card>> CardPile::peek(std::size_t index) const {
        if (this->cards.size() > index) {
            return this->cards[index];
        } else {
            return {};
        }
    }

    CardPile *CardPile::split(std::size_t amount) {
        auto newPile = new CardPile();

        for (std::size_t i = 0; i < amount; i++) {
            // get the card from top of pile
            std::shared_ptr<Card> currentTop = std::move(this->cards[0]);

            // remove now-null item from this->cards
            this->cards.pop_front();

            // add it to new pile (in reverse order)
            newPile->cards.push_back(currentTop);
        }

        return newPile;
    }

    std::shared_ptr<Card> CardPile::takeTop() {
        if (this->empty()) return nullptr;

        // yoink unique pointer from the pile
        std::shared_ptr<Card> top = std::move(this->cards[0]);

        // remove now-null pointer
        this->cards.pop_front();

        return top;
    }

    std::ostream& operator<<(std::ostream& os, const Card& card) {
        return os << card.face << card.suit;
    }
}