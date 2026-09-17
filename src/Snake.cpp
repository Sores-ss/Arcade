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
        const std::size_t rectSize = 50;
        const std::size_t mapX = 650;
        const std::size_t mapY = 300;
        const std::size_t mapWidth = 10;
        const std::size_t mapHeight = 10;
        bool mapInit = false;

        if (!mapInit) {
            for (std::size_t i = 0; i < mapHeight; i++) {
                for (std::size_t j = 0; j < mapWidth; j++) {
                    IRect *tile = display->createRect(rectSize, rectSize, mapX + i * rectSize,
                        mapY + j * rectSize);
                    if ((i + j) % 2 == 0)
                        display->setTexture(tile, {"", 10, 150, 40, 255});
                    else
                        display->setTexture(tile, {"", 35, 180, 55, 255});
                    _tiles.push_back(tile);
                }
            }
            mapInit = true;
        }
        for (IRect *tile : _tiles)
            display->displayRect(tile);
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
