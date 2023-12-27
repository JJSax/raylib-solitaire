#pragma once

#include <array>
#include <unordered_map>
#include <map>
#include <algorithm>
#include <random>

#include "card.hpp"
#include "except.hpp"
#include "sltconfig.hpp"

namespace solitaire {
    class Game {
    public:
        /// @brief Destroys the game and all Cards allocated for it.
        ~Game();

        /// @brief Creates and fully initializes a game.
        /// @tparam URNG The uniform PRNG type to shuffle the cards with.
        /// @param rand The uniform PRNG instance to use.
        /// @return The shuffled and dealt Game.
        template<typename URNG>
        static Game *createAndDealGame(URNG& rand) {
            Game *g = new Game();
            g->shuffleStock(rand);
            g->dealGame();
            return g;
        }

        /// @brief Deals the closed and open tableaus to start the game.
        /// @throws solitaire::NotEnoughCardsException If the deck has too few cards to deal a full game;
        /// should only happen when either NUM_TABLEAUS is increased, or the Suit or Face enums are changed.
        void dealGame();

        /// @brief Checks if there are any cards in the stock.
        /// @return false if the stock is empty; true otherwise.
        bool hasStock() const noexcept;

        /// @brief Turns a card from the stock onto the waste.
        /// @throws solitaire::NotEnoughCardsException If the stock is empty.
        void turnStock();

        /// @brief Turns the waste pile onto the stock.
        /// @throws std::logic_error if the stock is not empty.
        void returnWasteToStock();

        /// @brief Checks the card on top of the waste.
        /// @return nullptr if the waste is empty; a pointer to the top card otherwise.
        const Card *peekWaste() const noexcept;

        /// @brief Gets the top card of the foundation for the Suit s.
        /// @param s Suit whose foundation's top card is desired.
        /// @throws std::out_of_range if s is not one of the 4 core Suits.
        /// @return nullptr if the foundation is empty; a pointer to the top card otherwise.
        const Card *peekFoundation(Suit s) const;

        /// @brief Gets the currently held card pile.
        /// @return The pile of currently held cards.
        const CardPile& getHeldCards();

        /// @brief Takes the card on top of the waste into the held cards.
        /// @throws solitaire::NotEnoughCardsException If the waste is empty.
        /// @throws std::logic_error If there are already cards being held.
        void takeWaste();

        /// @brief Takes the card on top of the chosen foundation into the held cards.
        /// @param s solitaire::Suit Which foundation to take from.
        /// @throws solitaire::NotEnoughCardsException If the chosen foundation is empty.
        /// @throws std::logic_error If there are already cards being held.
        void takeFoundation(Suit s);

        /// @brief Takes the amount top cards of the open tableau at index index.
        /// @param index Which tableau to take cards from.
        /// @param amount How many cards to take from the tableau.
        /// @throws solitaire::NotEnoughCardsException If there arent enough cards in the open tableau to take.
        /// @throws std::logic_error If there are already cards being held.
        void takeTableau(std::size_t index, std::size_t amount);

        /// @brief Stacks the 1 element card pile on top of waste.
        /// @throws std::logic_error If there are no held cards.
        void returnHeldCards();

        /// @brief Gets the open tableau at index index.
        /// @param index Which tableau to fetch.
        /// @return The open tableau at the given index.
        const CardPile& getOpenTableau(std::size_t index) const;

        /// @brief Gets the size of the closed tableau at index index.
        /// @param index Which tableau to access.
        /// @return The size of the selected closed tableau.
        std::size_t getClosedTableauSize(std::size_t index) const;

        /// @brief Flips the top card of the closed tableau and puts it into the open tableau.
        /// @param index Which tableau to flip a card from.
        /// @throws std::out_of_range if there is no tableau at index index.
        /// @throws std::logic_error if either the open tableau at index is not empty,
        /// or if the closed tableau at index is empty.
        void turnClosedTableauTop(std::size_t index);

        /// @brief Attempts to stack the held CardPile on top of the open tableau at index index.
        /// @param index Which tableau to stack onto.
        /// @throws std::out_of_range If there is no tableau at index index.
        /// @throws solitaire::NotEnoughCards If the card pile is empty.
        /// @throws solitaire::MismatchedSuitsException If the base card of the given pile has the same
        /// color as the top of the chosen open tableau.
        /// @throws solitaire::NonSequentialFacesException If the base card of the given pile cannot
        /// follow the top card of the given open tableau.
        /// @throws solitaire::InvalidCardPlacementException If the open tableau is empty and the base
        /// of the pile is not a king.
        void stackTableau(std::size_t index);

        /// @brief Attempts to stack the held CardPile on top of the foundation for the given Suit.
        /// @param suit The suit whose foundation will be stacked onto.
        /// @throws std::out_of_range If the given suit is invalid or the special value Suit::END.
        /// @throws solitaire::NotEnoughCardsException If the held cards pile is empty.
        /// @throws solitaire::TooManyCards If the held cards pile contains more than 1 card.
        void stackFoundation(Suit suit);

    private:
        Game();

        template<typename URNG>
        void shuffleStock(URNG& rand) {
            std::shuffle(this->stock.begin(), this->stock.end(), rand);
        }

        std::array<CardPile, NUM_TABLEAUS> openTableau;
        std::array<CardPile, NUM_TABLEAUS> closedTableau;
        std::unordered_map<Suit, CardPile> foundation;
        CardPile stock;
        CardPile waste;
        CardPile heldCards;

        enum class PossibleHeldCardsSource {
            WASTE,
            TABLEAU,
            FOUNDATION
        } heldCardsSource;
        union {
            std::size_t tableauIndex;
            Suit foundationSuit;
        } heldSourcePileExtra;

        std::vector<Card *> allCards;

        void deal(CardPile& onto);

        // table init functions
        void initFullDeckInOrder() noexcept;
        void initFoundations() noexcept;
        void dealClosedTableau();
        void dealOpenTableau();

        void throwIfAttemptingToHoldMoreCards();
    };
}