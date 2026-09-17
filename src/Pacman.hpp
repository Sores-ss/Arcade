/*
** EPITECH PROJECT, 2025
** G-OOP-400-LIL-4-1-arcade-9
** File description:
** Pacman.hpp
*/

#pragma once

#include "IGameModule.hpp"
#include <vector>

namespace arcade {
    class Pacman : public IGameModule {
        public:
            Pacman() = default;
            ~Pacman() = default;
            void run(IDisplayModule *display) override;
            void pause() override;
            void stop() override;
        private:
            std::vector<IRect *> _tiles;
    };
}
