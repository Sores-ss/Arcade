/*
** EPITECH PROJECT, 2025
** G-OOP-400-LIL-4-1-arcade-9
** File description:
** IDisplayModule.hpp
*/

#pragma once

#include <iostream>
#include "IRect.hpp"

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
            virtual void setText(IRect *rect, std::string text, Texture_t texture) = 0;
            virtual void init(std::string, size_t, size_t) = 0;
            virtual void stop() = 0;
            virtual void render() = 0;
            virtual EEvent pollEvent() = 0;
            virtual const std::string &getName() const = 0;
            virtual void clear() const = 0;
            virtual const EType &getType() const = 0;
            virtual void displayRect(IRect *b) const = 0;
            virtual void setTexture(IRect *rect, Texture_t texture) = 0;
            virtual const IRect *createRect(size_t width, size_t height, size_t x, size_t y) = 0;
    };
}
