extern "C" {
#include <raylib.h>
}

#include <iostream>
#include <memory>
#include <algorithm>

#include "sltgraphics.hpp"

using namespace solitaire;
using namespace std;

void detectClick(Game& game) {
    // TODO
}

void drawGame(const Game& game) {
}

int main() {
    auto rng = std::default_random_engine();
    rng.seed(123UL);

    GraphicalGame *game = GraphicalGame::createAndDealGame(rng);
    // Game game;
    // game.shuffleStock(rng);
    // game.dealGame();

    // game.turnStock();
    // cout << (*game.peekWaste()) << endl;

    InitWindow(game->windowWidth(), game->windowHeight(), "Solitaire");
    while (!WindowShouldClose()) {
        BeginDrawing();
            ClearBackground(RAYWHITE);
            game->draw();
        EndDrawing();

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            game->detectClick(GetMousePosition());
        }
    }
    CloseWindow();

    delete game;
    return 0;
}