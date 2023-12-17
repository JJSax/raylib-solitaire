#include <iostream>

extern "C" {
#include <raylib.h>
}

#include <memory>
#include <algorithm>

#include "card.hpp"

using namespace solitaire;
using namespace std;

int main() {
    CardPile deck;
    for (int s = CLUBS; s <= SPADES; s++) {
        Suit mySuit = static_cast<Suit>(s);
        for (int f = ACE; f <= KING; f++) {
            Card c(static_cast<Suit>(s), static_cast<Face>(f));
            deck.add(c);
        }
    }

    std::random_shuffle(deck.begin(), deck.end());
    for (auto card : deck) {
        cout << *card << endl;
    }

    return 0;
}