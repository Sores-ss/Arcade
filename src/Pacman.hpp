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
#include <array>

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
            int handleX(int x) const;
            int handleY(int y) const;
            void displayGhosts(IDisplayModule *display);
            void moveGhosts(IDisplayModule *display);
            bool isWalkableTile(int x, int y) const;
            bool canGhostMoveTo(size_t ghostIndex, int x, int y) const;
            bool isInCage(int x, int y) const;
            IRect *_rectBase;
            IRect *_rectScore;
            IRect *_rectWord;
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
            int _nextDirX = 1;
            int _nextDirY = 0;
            std::chrono::steady_clock::time_point _lastMove = std::chrono::steady_clock::now();
            std::chrono::steady_clock::time_point _ghostLastMove = std::chrono::steady_clock::now();
            std::chrono::steady_clock::time_point _ghostCageStart = std::chrono::steady_clock::now();
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
            std::string _state = "RUNNING";
            std::string _pacmanDirection = "./assets/pacman_right.png";
            void changePacman();
            struct Ghost {
                IRect *rect = nullptr;
                int x = 0;
                int y = 0;
                int dirX = 1;
                int dirY = 0;
                bool inCage = true;
                std::string texture;
            };
            std::array<Ghost, 4> _ghosts;
            int _cageLeft = 10;
            int _cageRight = 18;
            int _cageTop = 13;
            int _cageBottom = 17;
            int _cageExitX = 13;
            bool _initGhostPosition = false;
            bool _running = false;
            int _superSonic = 0;
            int _ghostSpeed = 200;
            std::chrono::steady_clock::time_point _superSonicStart;
    };
}
