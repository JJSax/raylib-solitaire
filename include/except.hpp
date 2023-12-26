#pragma once

#include <exception>

namespace solitaire {
    class NotEnoughCardsException : public std::exception {};
    class TooManyCardsException : public std::exception {};

    class InvalidStateException : public std::exception {
        std::string msg;

    public:
        InvalidStateException(std::string message): msg(message) {}

        const char *what() {
            return msg.c_str();
        }
    };

    class InvalidCardPlacementException : public std::exception {};
    class MismatchedSuitsException : public InvalidCardPlacementException {};
    class NonSequentialFacesException : public InvalidCardPlacementException {};
}