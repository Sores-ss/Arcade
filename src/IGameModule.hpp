/*
** EPITECH PROJECT, 2025
** G-OOP-400-LIL-4-1-arcade-9
** File description:
** IGame.hpp
*/

#pragma once
#include "IDisplayModule.hpp"

namespace arcade {
    class IGame {
    public:
        IGame() = default;
        virtual ~IGame() = default;
        virtual const EType &getType() const = 0;
    };
}