/*
** EPITECH PROJECT, 2025
** G-OOP-400-LIL-4-1-arcade-9
** File description:
** Snake.cpp
*/

#include "Snake.hpp"

__attribute__((constructor)) void create(void)
{
    std::cout << "Opening snake..." << std::endl;
}

__attribute__((destructor)) void destroy(void)
{
    std::cout << "Closing snake..." << std::endl;
}

extern "C" {
    arcade::Snake *myEntryPoint(void)
    {
        std::cout << "Loading snake.." << std::endl;
        return new arcade::Snake();
    }

    const arcade::EType getLibType(void)
    {
        return arcade::EType::GAME;
    }

    const std::string getLibName(void)
    {
        return "Snake";
    }
}

namespace arcade {
    void Snake::run(IDisplayModule *display)
    {
        (void)display;
    }

    void Snake::pause()
    {
        std::cout << "snake paused" << std::endl;
    }

    void Snake::stop()
    {
        std::cout << "snake stopped" << std::endl;
    }
}
