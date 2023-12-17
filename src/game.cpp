#include "game.hpp"
#include "except.hpp"

namespace solitaire {
    bool suitsCanAlternate(Suit s1, Suit s2) noexcept {
        // works due to the ordering of the suits: endpoints add
        // to 3 and are both black, midpoints add to 3 and are both red.
        // any other pair will not add up to three, therefore we have
        // this function
        return s1 + s2 != 3;
    }

    bool canStackInTableau(const Card& oldTop, const Card& newTop) noexcept {
        return (oldTop.face - newTop.face) == 1 && suitsCanAlternate(oldTop.suit, newTop.suit);
    }

    bool canStackInFoundation(Suit foundationSuit, const Card& oldTop, const Card& newTop) noexcept {
        return (newTop.face - oldTop.face) == 1 && newTop.suit == foundationSuit;
    }
}