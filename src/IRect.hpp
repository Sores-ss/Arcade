/*
** EPITECH PROJECT, 2025
** G-OOP-400-LIL-4-1-arcade-9
** File description:
** IRect.hpp
*/

#pragma once

#include <iostream>

namespace arcade {
    typedef struct Texture_s {
        std::string filepath = "";
        u_int8_t r;
        u_int8_t g;
        u_int8_t b;
        u_int8_t a;
    } Texture_t;

    typedef struct rectInfo_s {
        std::size_t x;
        std::size_t y;
        std::size_t w;
        std::size_t h;
    } rectInfo_t;

    class IRect {
        public:
            IRect() = default;
            virtual ~IRect() = default;
            virtual rectInfo_t getRectInfo() const = 0;
            virtual bool isMouseOver() const = 0;
            virtual void setPosition(std::size_t x, std::size_t y) = 0;
    };
}