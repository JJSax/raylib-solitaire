#include <raylib.h>

#include <iostream>
#include <memory>
#include <algorithm>

#include "sltgraphics.hpp"

using namespace solitaire;
using namespace std;

std::unique_ptr<GraphicalGame> solitaire::debugScript(std::unique_ptr<GraphicalGame> game) {
    // while (game->game->hasStock()) {
    //     game->game->turnStock();
    // }
    return game;
}

int main() {
    InitWindow(TARGET_RESOLUTION.x, TARGET_RESOLUTION.y, "Solitaire");
    SetTargetFPS(60);

    std::unique_ptr<GraphicalGame> game = nullptr;
    try {
        auto rng = std::default_random_engine();
        rng.seed(123UL);

        game = GraphicalGame::create(rng);
        game->calculateBounds();
        game = solitaire::debugScript(std::move(game));

        while (!WindowShouldClose()) {
            BeginDrawing();
                ClearBackground(BACKGROUND_COLOR);
                game->render();
            EndDrawing();

            auto mousePos = GetMousePosition();
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                game->handleClick(mousePos);
            }
            if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
                game->handleDrag(mousePos);
            }
            if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
                game->releaseDrag(mousePos);
            }
        }
    } catch (const std::exception& e) {
        cerr << e.what() << endl;
    }
    // must deinit game while window is still open
    game.release();
    CloseWindow();
    return 0;
}