/*
** EPITECH PROJECT, 2025
** G-OOP-400-LIL-4-1-arcade-9
** File description:
** Snake.hpp
*/

#include "IGameModule.hpp"

namespace arcade {
    class Snake : public IGameModule {
        public:
            Snake() = default;
            ~Snake() = default;
            void run(IDisplayModule *display) override;
            void pause() override;
            void stop() override;
    };
}
