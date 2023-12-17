#include <iostream>

extern "C" {
#include <raylib.h>
}

#include <memory>
#include <algorithm>

#include "game.hpp"
#include "card.hpp"

using namespace solitaire;
using namespace std;

int main() {
    auto rng = std::default_random_engine();
    rng.seed(123UL);
    Game<decltype(rng)> game(rng);
    game.turnStock();
    cout << *game.peekWaste().value().lock() << endl;

    return 0;
}