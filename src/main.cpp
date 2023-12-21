#include <raylib.h>

#include <iostream>
#include <memory>
#include <algorithm>

#include "sltgraphics.hpp"

using namespace solitaire;
using namespace std;


int main() {
    InitWindow(TARGET_RESOLUTION.x, TARGET_RESOLUTION.y, "Solitaire");

    std::unique_ptr<GraphicalGame> game = nullptr;
    try {
        auto rng = std::default_random_engine();
        rng.seed(123UL);

        game = GraphicalGame::create(rng);
        game->calculateBounds();

        while (!WindowShouldClose()) {
            BeginDrawing();
                ClearBackground(BACKGROUND_COLOR);
                game->render();
            EndDrawing();

            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                game->detectClick(GetMousePosition());
            }
        }
    } catch (const std::exception& e) {
        cout << e.what() << endl;
    }
    // must deinit game while window is still open
    game.release();
    CloseWindow();
    return 0;
}