#pragma once

#include <deque>
#include <optional>
#include <memory>
#include <iosfwd>

namespace solitaire {
    // weakest to strongest in poker order
    enum Suit {
        CLUBS,
        DIAMONDS,
        HEARTS,
        SPADES
    };

    std::ostream& operator<<(std::ostream& os, const Suit& suit);

    // in order for foundation
    enum Face {
        ACE = 1,
        TWO,
        THREE,
        FOUR,
        FIVE,
        SIX,
        SEVEN,
        EIGHT,
        NINE,
        TEN,
        JACK,
        QUEEN,
        KING
    };

    std::ostream& operator<<(std::ostream& os, const Face& face);

    class Card {
    private:
        bool faceUp;

    public:
        const Face face;
        const Suit suit;

        Card(Face f, Suit s): Card(f, s, false) {}
        Card(Face f, Suit s, bool faceUp): face(f), suit(s), faceUp(faceUp) {}

        void setFaceUp(bool faceUp);
        bool isFaceUp();

        friend std::ostream& operator<<(std::ostream& os, const Card& card);
    };

    class CardPile {
        std::deque<std::shared_ptr<Card>> cards;
    public:
        virtual ~CardPile() {
            // NOP, shared pointers self destruct when out of scope
        }

        void add(const Card& c);

        template<typename Iterator>
        void addRange(Iterator start, Iterator end) {
            this->cards.push_back(start, end);
        }

        bool empty();
        std::size_t size();

        auto begin() {
            return this->cards.begin();
        }

        auto end() {
            return this->cards.end();
        }

        [[nodiscard]] std::optional<const std::weak_ptr<Card>> peek(std::size_t index=0) const;

        // splits the deck, with the range [0, amount) being returned in a
        // new CardPile, and the remaining cards staying put as this deck's cards.
        [[nodiscard]] CardPile *split(std::size_t amount);

        // returns nullptr if there is no top
        [[nodiscard]] std::shared_ptr<Card> takeTop();
    };
}