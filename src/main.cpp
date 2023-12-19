extern "C" {
#include <raylib.h>
}

#include <iostream>
#include <memory>
#include <algorithm>

#include "sltgraphics.hpp"

using namespace solitaire;
using namespace std;

// TODO calculate these in runtime
// TODO shrink window size by 2 if it wouldnt fit in the monitor
const int baseWindowWidth = 1280;
const int baseWindowHeight = 720;
float gameScale = 1.0f;

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

    InitWindow(baseWindowWidth, baseWindowHeight, "Solitaire");
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

    return 0;
}