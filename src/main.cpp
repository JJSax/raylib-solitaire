#include <raylib.h>

#include <iostream>
#include <memory>
#include <algorithm>
#include <chrono>

#include "sltgraphics.hpp"

using namespace solitaire;
using namespace std;

int secondsSinceEpoch() {
    auto now = std::chrono::system_clock::now();
    constexpr auto n = std::chrono::system_clock::period::num;
    constexpr auto d = std::chrono::system_clock::period::den;
    return now.time_since_epoch().count() * n / d;
}

int main() {
    InitWindow(TARGET_RESOLUTION.x, TARGET_RESOLUTION.y, "Solitaire");
    SetTargetFPS(60);

    try {
        GraphicalGame game(secondsSinceEpoch());

        while (!WindowShouldClose()) {

            game.update();

            BeginDrawing();
                ClearBackground(BACKGROUND_COLOR);
                game.render();
            EndDrawing();

            auto mousePos = GetMousePosition();
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                game.handleClick(mousePos);
            }
            if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
                game.handleDrag(mousePos);
            }
            if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
                game.releaseDrag(mousePos);
            }
        }
    } catch (const std::exception& e) {
        cerr << e.what() << endl;
    }
    // must deinit game while window is still open
    CloseWindow();
    return 0;
}