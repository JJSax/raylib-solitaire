#pragma once

#include <exception>

namespace solitaire {
    class NotEnoughCardsException : std::exception {};

    class InvalidCardPlacementException : std::exception {};
    class MismatchedColorsException : InvalidCardPlacementException {};
    class NonSequentialNumbersException : InvalidCardPlacementException {};
}