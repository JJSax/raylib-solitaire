#pragma once

#include <exception>

namespace solitaire {
    class NotEnoughCardsException : public std::exception {};
    class TooManyCardsException : public std::exception {};

    class InvalidCardPlacementException : public std::exception {};
    class MismatchedSuitsException : public InvalidCardPlacementException {};
    class NonSequentialFacesException : public InvalidCardPlacementException {};
}