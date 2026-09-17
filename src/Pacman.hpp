/*
** EPITECH PROJECT, 2025
** G-OOP-400-LIL-4-1-arcade-9
** File description:
** Pacman.hpp
*/

#pragma once

#include "IGameModule.hpp"
#include <chrono>
#include <vector>

#define TILE_SIZE 15
#define MAP_WIDTH 28
#define MAP_HEIGHT 31

namespace arcade {
    class Pacman : public IGameModule {
        public:
            Pacman() = default;
            ~Pacman() = default;
            void run(IDisplayModule *display) override;
            void pause() override;
            void stop() override;
        private:
            int _score = 0;
            size_t _pacmanStartX = 13;
            size_t _pacmanStartY = 17;
            void loadMap(IDisplayModule *display);
            void displayPacman(IDisplayModule *display);
            void updateDirection(EEvent event);
            void movePacman();
            IRect *_rectBase;
            IRect *_rectScore;
            IRect *_pacman;
            struct Tile {
                IRect *rect;
                bool visible;
            };
            std::vector <Tile> _map;
            Size _windowSize;
            Bounds _rectBounds;
            int _dirX = 1;
            int _dirY = 0;
            std::chrono::steady_clock::time_point _lastMove = std::chrono::steady_clock::now();
            std::vector<std::string> map = {
                "############################",
                "#............##.......0....#",
                "#.####.#####.##.#####.####.#",
                "#.####.#####.##.#####.####.#",
                "#.####.#####.##.#####.####.#",
                "#...0......................#",
                "#.####.##.########.##.####.#",
                "#.####.##.########.##.####.#",
                "#......##....##....##......#",
                "######.#####.##.#####.######",
                "######.#####.##.#####.######",
                "######.##..........##.######",
                "######.##.########.##.######",
                "######.##.########.##.######",
                "#.........########....0....#",
                "######.##.########.##.######",
                "######.##.########.##.######",
                "######.##..........##.######",
                "######.##.########.##.######",
                "######.##.########.##.######",
                "#............##............#",
                "#.####.#####.##.#####.####.#",
                "#.####.#####.##.#####.####.#",
                "#...##................##...#",
                "###.##.##.########.##.##.###",
                "###.##.##.########.##.##.###",
                "#...0..##....##....##......#",
                "#.##########.##.##########.#",
                "#.##########.##.##########.#",
                "#..........................#",
                "############################",
            };
            std::vector<std::vector<IRect *>> _gumMap;
            bool _paused = false;
    };
}
