#pragma once

#include <array>
#include <map>
#include <algorithm>
#include <random>
#include <optional>

#include "card.hpp"
#include "except.hpp"


namespace solitaire {
    bool suitsCanAlternate(Suit s1, Suit s2) noexcept;
    bool canStackInTableau(const CardPile& cardPile, const Card& newCard) noexcept;
    bool canStackInFoundation(Suit foundationSuit, const CardPile& pile, const Card& newCard) noexcept;

    class Game {
    public:
        static const int NUM_TABLEAUS = 7;

        Game();
        virtual ~Game();

        template<typename URNG>
        void shuffleStock(URNG& rand) {
            std::shuffle(this->stock.begin(), this->stock.end(), rand);
        }

        void dealGame();

        bool hasStock() const noexcept;
        void turnStock();
        const Card *peekWaste() const noexcept;
        Card *takeWaste();
        const CardPile& getOpenTableau(std::size_t index) const;
        std::size_t getClosedTableauSize(std::size_t index) const;

        // these transfer cards out of card pile
        bool stackTableau(std::size_t index, CardPile& cards);
        bool stackFoundation(Suit suit, CardPile& cards);

    private:
        std::array<CardPile, NUM_TABLEAUS> openTableau;
        std::array<CardPile, NUM_TABLEAUS> closedTableau;
        std::unordered_map<Suit, CardPile> foundation;
        CardPile stock;
        CardPile waste;
        std::vector<Card *> allCards;

        void deal(CardPile& onto);

        // table init functions
        void initFullDeckInOrder() noexcept;
        void initFoundations() noexcept;
        void dealClosedTableau();
        void dealOpenTableau();
    };
}