#pragma once

#include <raylib.h>

namespace solitaire {
    static const int NUM_TABLEAUS = 7;

    const Vector2 TARGET_RESOLUTION = {1280, 720};

    const char CARD_TEXTURE_PATH_PREFIX[] = "assets/images/";

    // path to the image file containing the back of cards
    const char CARD_BACK_TEXTURE_PATH_SUFFIX[] = "back/A.png";

    const Color BACKGROUND_COLOR = DARKGREEN;

    // how many pixels cards on top of each other will be offset by
    const int STACKED_DISPLACEMENT = 30;
    // same, but for face down cards
    const int FACE_DOWN_STACKED_DISPLACEMENT = 15;

    // how much of the card *must* be visible under the most strenuous circumstance
    // (full closed tableau + full open tableau)
    const int MIN_CARD_SHOWN_SIZE = 2 * STACKED_DISPLACEMENT;

    // in pixels
    const int TINY_SPACING = 20;
    const int SMALL_SPACING = 50;
    const int MED_SPACING = 150;
}