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
        ENTER,
        CLICK
    };

    class IDisplayModule {
        public :
            IDisplayModule() = default;
            virtual ~IDisplayModule() = default;
            virtual const std::string getName() const = 0;
            virtual const EType getType() const = 0;
            virtual void init(std::string name, Size size) = 0;
            virtual const Size getWindowSize() const = 0;
            virtual void stop() = 0;
            virtual void setBackground(Texture texture) = 0;
            virtual IRect *createRect(Bounds bounds) = 0;
            virtual void setTexture(IRect &rect, Texture texture) = 0;
            virtual void setBorder(IRect &rect, Texture texture) = 0;
            virtual void setText(IRect &rect, std::string text, Texture texture) = 0;
            virtual void displayRect(IRect &rect) const = 0;
            virtual void setMusic(std::string filepath) = 0;
            virtual void playSound(std::string filepath) const = 0;
            virtual void render() = 0;
            virtual const EEvent pollEvent() = 0;
            virtual void clearWindow() const = 0;
    };
};
