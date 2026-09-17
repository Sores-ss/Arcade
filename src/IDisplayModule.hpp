/*
** EPITECH PROJECT, 2025
** G-OOP-400-LIL-4-1-arcade-9
** File description:
** IDisplayModule.hpp
*/

#pragma once

#include <iostream>

namespace arcade {
    enum EType {
        Graphical,
        Game
    };

    class IButton {
        public:
            IButton() = default;
            virtual ~IButton() = default;
            virtual bool is_clicked() const = 0;
            virtual bool is_selected() const = 0;
            virtual void setText(std::string text) const = 0;
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