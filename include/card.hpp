#pragma once

#include <deque>
#include <optional>
#include <memory>
#include <iosfwd>


namespace solitaire {
    // weakest to strongest in poker order
    enum class Suit {
        CLUBS,
        FIRST = CLUBS,
        DIAMONDS,
        HEARTS,
        SPADES,
        END,
        COUNT = END
    };

    /**
     * @brief Converts a Suit into a char.
     * CLUBS becomes 'c', DIAMONDS becomes 'd', HEARTS becomes 'h', and SPADES becomes 's'.
     * @param s Suit to convert.
     * @return char The char representation of the Suit.
     */
    char suitToChar(Suit s);
    Suit& operator++(Suit& s);
    Suit operator++(Suit& s, int);
    std::ostream& operator<<(std::ostream& os, const Suit& suit) noexcept;

    // in order for foundation
    enum class Face {
        ACE = 1,
        FIRST = ACE,
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
        END,
        COUNT = END - FIRST
    };

    /**
     * @brief Converts a Face into a char.
     * ACE becomes 'A',
     * TWO through NINE become '2' through '9',
     * TEN becomes 'X',
     * JACK becomes 'J',
     * QUEEN becomes 'Q',
     * KING becomes 'K'.
     * @param f Face to convert.
     * @return char The char representation of the Face.
     */
    char faceToChar(Face f);
    Face& operator++(Face& f);
    Face operator++(Face& f, int);
    std::ostream& operator<<(std::ostream& os, const Face& face) noexcept;

    /**
     * @brief The representation of a Card, with a Suit and a Face.
     */
    class Card {
    public:
        const Face face;
        const Suit suit;

        Card(Face f, Suit s) noexcept: face(f), suit(s) {}

        friend std::ostream& operator<<(std::ostream& os, const Card& card) noexcept;
    };

    /**
     * @brief The representation of a stack of cards.
     */
    class CardPile {
        std::deque<Card *> cards;
    public:
        /**
         * @brief Adds a Card to the top of the pile.
         * @param c The Card to add.
         */
        void add(Card *c) noexcept;

        /**
         * @brief Checks if the pile is empty.
         * @return true If the pile is empty.
         * @return false If there are cards in the pile.
         */
        bool empty() const noexcept;

        /**
         * @brief Gets the number of cards in the pile.
         * @return std::size_t The number of cards in the pile.
         */
        std::size_t size() const noexcept;

        auto begin() noexcept {
            return this->cards.begin();
        }

        auto end() noexcept {
            return this->cards.end();
        }

        auto begin() const noexcept {
            return this->cards.cbegin();
        }

        auto end() const noexcept {
            return this->cards.cend();
        }

        auto rbegin() noexcept {
            return this->cards.rbegin();
        }

        auto rend() noexcept {
            return this->cards.rend();
        }

        auto rbegin() const noexcept {
            return this->cards.crbegin();
        }

        auto rend() const noexcept {
            return this->cards.crend();
        }


        /**
         * @brief Returns the Card on an index from the top of the pile, if any.
         * @param index The position from the top of the pile to check; 0 or omitted for the top of the pile.
         * @return nullptr If there is no card at the given index.
         * @return const Card* The non-nullptr Card * at that position of the pile.
         */
        [[nodiscard]] const Card *peek(std::size_t index=0) const noexcept;

        /**
         * @brief Returns the Card on the bottom of the pile, if any.
         * @return nullptr If the pile is empty.
         * @return const Card* The non-nullptr Card * at the base of the pile.
         */
        [[nodiscard]] const Card *peekBase() const noexcept;

        /**
         * @brief Splits the pile in two, with the index range [0, amount) being returned
         * in a new CardPile, and the remaining Cards staying put as this pile's cards.
         * The new CardPile was allocated with new and MUST be deleted after use.
         * @param amount The amount of cards to put in the new pile.
         * @throws solitaire::NotEnoughCardsException If there are not enough cards in the pile.
         * @return CardPile* The newly allocated pile.
         */
        [[nodiscard]] CardPile *split(std::size_t amount);

        /**
         * @brief Takes all cards from newTop and moves them to the top of this pile.
         */
        void stack(CardPile& newTop) noexcept;

        /**
         * @brief Removes the top card of the pile and returns it.
         * @throws solitaire::NotEnoughCardsException If the pile is empty.
         * @return Card* The removed Card.
         */
        [[nodiscard]] Card *takeTop();

        /**
         * @brief Removes the bottom card of the pile and returns it.
         * @throws solitaire::NotEnoughCardsException If the pile is empty.
         * @return Card* The removed Card.
         */
        [[nodiscard]] Card *takeBase();

        /**
         * @brief Removes all cards from this pile and stacks them in reverse order on top of newBase.
         * @param newBase The destination pile.
         */
        void turnOnto(CardPile& newBase);
    };
}