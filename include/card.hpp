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

    std::ostream& operator<<(std::ostream& os, const Suit& suit) noexcept;

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

    std::ostream& operator<<(std::ostream& os, const Face& face) noexcept;

    class Card {
    private:
        bool faceUp;

    public:
        const Face face;
        const Suit suit;

        Card(Face f, Suit s) noexcept: Card(f, s, false) {}
        Card(Face f, Suit s, bool faceUp) noexcept: face(f), suit(s), faceUp(faceUp) {}

        void setFaceUp(bool faceUp) noexcept;
        bool isFaceUp() const noexcept;

        friend std::ostream& operator<<(std::ostream& os, const Card& card) noexcept;
    };

    class CardPile {
        std::deque<std::shared_ptr<Card>> cards;
    public:
        virtual ~CardPile() noexcept {
            // NOP, shared pointers self destruct when out of scope
        }

        void add(const Card& c) noexcept;
        void add(std::shared_ptr<Card> c) noexcept;

        template<typename Iterator>
        void addRange(Iterator start, Iterator end) noexcept {
            this->cards.push_back(start, end);
        }

        bool empty() const noexcept;
        std::size_t size() const noexcept;

        auto begin() noexcept {
            return this->cards.begin();
        }

        auto end() noexcept {
            return this->cards.end();
        }

        [[nodiscard]] std::optional<const std::weak_ptr<Card>> peek(std::size_t index=0) const noexcept;

        // splits the deck, with the range [0, amount) being returned in a
        // new CardPile, and the remaining cards staying put as this deck's cards.
        // throws if amount > this->size()
        [[nodiscard]] CardPile *split(std::size_t amount);

        // throws if pile is empty
        [[nodiscard]] std::shared_ptr<Card> takeTop();
    };
}