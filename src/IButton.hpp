/*
** EPITECH PROJECT, 2025
** Visual Studio Live Share (Workspace)
** File description:
** IButton.hpp
*/

#pragma once

namespace arcade {
    class IButton {
        public:
            IButton() = default;
            virtual ~IButton() = default;
            virtual bool isMouseOver() const = 0;
            virtual void setText(std::string text) const = 0;
    };
}