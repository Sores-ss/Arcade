/*
** EPITECH PROJECT, 2025
** G-OOP-400-LIL-4-1-arcade-9
** File description:
** core.hpp
*/

#pragma once

#include <vector>
#include <iostream>
#include "IDisplayModule.hpp"

namespace arcade {
    class Core {
        public:
            Core() = default;
            ~Core() = default;
            static std::vector<std::string> initLibList(std::string lib);
            const EType getType(const std::string &lib);
    };
}