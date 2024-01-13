#pragma once


namespace config {
    /// @brief Defines how many cards to pull when pulling off of the stock pile.
    enum class wasteDifficulty {
        ONE,
        THREE
    };

    /// @brief Automatically place card from waste when clicked.
    bool autoplayFromWaste = false;
    /// @brief Automatically find a place for clicked tableau.
    bool autoplayFromTableau = false;
    /// @brief Automatically put clicked card onto foundation.
    bool autoplayToFoundation = true;
    /// @brief Automatically flip top card of hidden tableau stack when it's exposed.
    bool autoplayClosedTableauTop = true;

    //ms click vs drag
    int framesToIgnoreClick = 10;

}
