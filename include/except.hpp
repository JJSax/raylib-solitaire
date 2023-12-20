#pragma once

#include <exception>

namespace solitaire {
    class NotEnoughCardsException : std::exception {};
    class TooManyCardsException : std::exception {};

    class InvalidStateException : std::exception {
        std::string msg;

    public:
        InvalidStateException(std::string message): msg(message) {}

        const char *what() {
            return msg.c_str();
        }
    };

    class InvalidCardPlacementException : std::exception {};
    class MismatchedSuitsException : InvalidCardPlacementException {};
    class NonSequentialFacesException : InvalidCardPlacementException {};
}