#pragma once


namespace config {
    /// @brief Defines how many cards to pull when pulling off of the stock pile.
    enum class wasteDifficulty {
        ONE,
        THREE
    };

    /// @brief Automatically place card from waste when clicked.
    inline bool autoplayFromWaste = true;
    /// @brief Automatically find a place for clicked tableau.
    inline bool autoplayFromTableau = true;
    /// @brief Automatically put clicked card onto foundation.
    inline bool autoplayToFoundation = true;
    /// @brief Automatically flip top card of hidden tableau stack when it's exposed.
    inline bool autoplayClosedTableauTop = true;

    /// @brief How many frames defines a click vs. a drag.
    inline int framesToIgnoreClick = 10;

}
