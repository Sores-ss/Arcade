/*
** EPITECH PROJECT, 2025
** G-OOP-400-LIL-4-1-arcade-9
** File description:
** IDisplayModule.hpp
*/

#pragma once

#include <iostream>
#include "IButton.hpp"

namespace arcade {
    enum EType {
        GRAPHICAL,
        GAME
    };

    class IDisplayModule {
        public :
            IDisplayModule() = default;
            virtual ~IDisplayModule() = default;
            virtual void init() = 0;
            virtual void stop() = 0;
            virtual const std::string &getName() const = 0;
            virtual const EType &getType() const = 0;
            virtual const IButton createButton(size_t width, size_t height, size_t x, size_t y) = 0;
    };
}