/*
** EPITECH PROJECT, 2025
** G-OOP-400-LIL-4-1-arcade-9
** File description:
** IGame.hpp
*/

#pragma once
#include "IDisplayModule.hpp"

namespace arcade {
    class IGameModule {
        public:
            IGameModule() = default;
            virtual ~IGameModule() = default;
            virtual void run(std::shared_ptr<IDisplayModule> display, const std::string name) = 0;
            virtual void pause() = 0;
            virtual bool changeDisplay() = 0;
    };
}