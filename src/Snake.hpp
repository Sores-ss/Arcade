/*
** EPITECH PROJECT, 2025
** G-OOP-400-LIL-4-1-arcade-9
** File description:
** Snake.hpp
*/

#pragma once

#include "IGameModule.hpp"
#include <vector>

namespace arcade {
    class Snake : public IGameModule {
        public:
            Snake() = default;
            ~Snake() = default;
            void run(IDisplayModule *display) override;
            void pause() override;
            void stop() override;
        private:
            std::vector<IRect *> _tiles;
    };
}
