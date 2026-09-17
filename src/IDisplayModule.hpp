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

    enum EEvent {
        UNDEFINED,
        QUIT,
        ESCAPE,
        UP,
        DOWN,
        RIGHT,
        LEFT,
        ENTER
    };

    class IDisplayModule {
        public :
            IDisplayModule() = default;
            virtual ~IDisplayModule() = default;
            virtual void init(std::string, size_t, size_t) = 0;
            virtual void stop() = 0;
            virtual void render() = 0;
            virtual EEvent pollEvent() = 0;
            virtual const void display(IButton *b) const = 0;
            virtual const IButton *createButton(size_t width, size_t height, size_t x, size_t y) = 0;
    };
}