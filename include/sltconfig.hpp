#pragma once

#include <raylib.h>

namespace solitaire {
    static const int NUM_TABLEAUS = 7;

    const Vector2 TARGET_RESOLUTION = {1280, 720};

    const char CARD_TEXTURE_PATH_PREFIX[] = "assets/images/";

    // path to the image file containing the back of cards
    const char CARD_BACK_TEXTURE_PATH_SUFFIX[] = "back/A.png";

    const float CARD_SCALE = 1.0f;

    const Color BACKGROUND_COLOR = DARKGREEN;

    const float CARD_SLOT_MIN_OVERLAP_AREA = 0.4f;

    // how many pixels cards on top of each other will be offset by
    const int STACKED_DISPLACEMENT = 30;
    // same, but for face down cards
    const int FACE_DOWN_STACKED_DISPLACEMENT = 15;

    // how much of the card *must* be visible under the most strenuous circumstance
    // (full closed tableau + full open tableau)
    const int MIN_CARD_SHOWN_SIZE = STACKED_DISPLACEMENT;

    // in pixels
    const float TINY_SPACING = 20;
    const float SMALL_SPACING = 50;
    const float MED_SPACING = 150;

    const Color TRANSPARENT_CARD_COLOR = Color {255, 255, 255, 127};
}