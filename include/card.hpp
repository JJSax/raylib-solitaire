#pragma once

#include <deque>
#include <optional>
#include <memory>
#include <iosfwd>

namespace solitaire {
    // weakest to strongest in poker order
    enum class Suit {
        CLUBS,
        DIAMONDS,
        HEARTS,
        SPADES,
        FIRST = CLUBS,
        LAST = SPADES
    };

    Suit& operator++(Suit& s);
    Suit operator++(Suit& s, int);
    std::ostream& operator<<(std::ostream& os, const Suit& suit) noexcept;

    // in order for foundation
    enum class Face {
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
        KING,
        FIRST = ACE,
        LAST = KING
    };

    Face& operator++(Face& f);
    Face operator++(Face& f, int);
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
        std::deque<Card *> cards;
    public:
        virtual ~CardPile() noexcept {
            // NOP, shared pointers self destruct when out of scope
        }

        void add(Card *c) noexcept;

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

        [[nodiscard]] const Card *peek(std::size_t index=0) const noexcept;
        [[nodiscard]] const Card *peekBase() const noexcept;

        // splits the deck, with the range [0, amount) being returned in a
        // new CardPile, and the remaining cards staying put as this deck's cards.
        // throws if amount > this->size()
        [[nodiscard]] CardPile *split(std::size_t amount);
        void stack(CardPile& newTop) noexcept;

        // these throw if pile is empty
        [[nodiscard]] Card *takeTop();
        [[nodiscard]] Card *takeBase();
    };
}