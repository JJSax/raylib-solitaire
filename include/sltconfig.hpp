#pragma once

extern "C" {
#include <raylib.h>
}

namespace solitaire {
    static const int NUM_TABLEAUS = 7;

    const Vector2 TARGET_RESOLUTION = {1280, 720};

    // suit, then face
    // suit for clubs is c, diamonds is d, hearts is h, and spades is s (all lowercase)
    // face for ace is A, 10 is X, jack is J, queen is Q, and king is K (all uppercase)
    // path is relative to CWD when running the executable
    // const char CARD_TEXTURE_PATH_PATTERN[] = "./assets/images/%c/%c.png";

    const char CARD_TEXTURE_PATH_PREFIX[] = "./assets/images/";
}