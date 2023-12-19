#pragma once

extern "C" {
#include <raylib.h>
}

namespace solitaire {
    const Vector2 TARGET_RESOLUTION = {1280, 720};

    // suit, then face
    // suit for clubs is c, diamonds is d, hearts is h, and spades is s (all lowercase)
    // face for ace is 1, 10 is X, jack is J, queen is Q, and king is K (all uppercase)
    const char TARGET_TEXTURE_NAME_PATTERN[] = "%c/%c.png";
}