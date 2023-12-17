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
    bool canStackInTableau(const Card& oldTop, const Card& newTop) noexcept;
    bool canStackInFoundation(Suit foundationSuit, const Card& oldTop, const Card& newTop) noexcept;

    template<typename URNG>
    class Game {
    public:
        static const int TABLEAU_SIZE = 7;

        Game(URNG& rand) {
            for (int s = CLUBS; s <= SPADES; s++) {
                Suit mySuit = static_cast<Suit>(s);
                for (int f = ACE; f <= KING; f++) {
                    Card c(static_cast<Face>(f), static_cast<Suit>(s));
                    this->stock.add(c);
                }
            }

            std::shuffle(this->stock.begin(), this->stock.end(), rand);
            for (int i = CLUBS; i <= SPADES; i++) {
                Suit s = static_cast<Suit>(i);
                this->foundation.emplace(s, CardPile());
            }
        }

        bool hasStock() const noexcept {
            return !this->stock.empty();
        }

        void turnStock() {
            if (this->stock.empty()) {
                throw NotEnoughCardsException();
            }
            auto card = this->stock.takeTop();
            this->waste.add(card);
        }

        std::optional<std::weak_ptr<Card>> peekWaste() const noexcept {
            return this->waste.peek();
        }

        std::shared_ptr<Card> takeWaste() {
            return this->waste.takeTop();
        }

    private:
        std::array<CardPile, TABLEAU_SIZE> tableau;
        std::unordered_map<Suit, CardPile> foundation;
        CardPile stock;
        CardPile waste;
    };
}