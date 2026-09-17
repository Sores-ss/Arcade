/*
** EPITECH PROJECT, 2025
** G-OOP-400-LIL-4-1-arcade-9
** File description:
** IRect.hpp
*/

#pragma once

#include <iostream>

namespace arcade {
    struct Texture {
        std::string filepath = "";
        u_int8_t r;
        u_int8_t g;
        u_int8_t b;
        u_int8_t a;
    };

    struct Size {
        size_t w;
        size_t h;
    };

    struct Position {
        size_t x;
        size_t y;
    };

    struct Bounds : Size, Position {
    };

class IRect {
        public:
            IRect() = default;
            virtual ~IRect() = default;
            virtual Bounds getBounds() const = 0;
            virtual bool isMouseOver() const = 0;
            virtual void setSize(Size size) = 0;
            virtual void setPosition(Position position) = 0;
            virtual void setText(std::string text, Texture texture) = 0;
            virtual void setBorder(Texture texture) = 0;
            virtual void setTexture(Texture texture) = 0;
            virtual void display() const = 0;
    };
};