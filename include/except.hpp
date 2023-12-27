#pragma once

#include <exception>

namespace solitaire {
    /// @brief Thrown when an operation requires more cards than there are available.
    class NotEnoughCardsException : public std::exception {};

    /// @brief Thrown when an operation requires fewer cards than there are available.
    class TooManyCardsException : public std::exception {};

    /// @brief Base class for two more specific cases of invalid card placement.
    class InvalidCardPlacementException : public std::exception {};

    /// @brief Thrown when the bottom pile's top card has an incompatible suit with the top pile's bottom card.
    class MismatchedSuitsException : public InvalidCardPlacementException {};

    /// @brief Thrown when the faces on the bottom pile's top card and on the top pile's bottom card are not sequential (according to the tableau/foundation rules).
    class NonSequentialFacesException : public InvalidCardPlacementException {};
}