/*
** EPITECH PROJECT, 2025
** G-OOP-400-LIL-4-1-arcade-9
** File description:
** core.hpp
*/

#pragma once

#include "IDisplayModule.hpp"
#include <map>
#include <iostream>

namespace arcade {
    class Core {
        public:
            Core() = default;
            ~Core() = default;
            static std::map<arcade::EType, std::string> initLibList();
    };
}