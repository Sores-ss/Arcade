/*
** EPITECH PROJECT, 2025
** G-OOP-400-LIL-4-1-arcade-9
** File description:
** Snake.hpp
*/

#pragma once

#include "IGameModule.hpp"
#include <random>
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
            struct Cell {
                std::size_t x;
                std::size_t y;
            };

            enum Direction {
                DIR_UP,
                DIR_DOWN,
                DIR_LEFT,
                DIR_RIGHT
            };

            static constexpr std::size_t _mapWidth = 20;
            static constexpr std::size_t _mapHeight = 20;
            static constexpr std::size_t _cellSize = 32;

            std::vector<IRect *> _tiles;
            std::vector<Cell> _snake;
            Cell _food = {0, 0};
            Direction _direction = DIR_RIGHT;
            Direction _nextDirection = DIR_RIGHT;
            std::mt19937 _rng;
            IDisplayModule *_display = nullptr;
            IRect *_scoreRect = nullptr;
            IRect *_statusRect = nullptr;
            bool _running = false;
            bool _paused = false;
            bool _gameOver = false;
            int _score = 0;

            void initBoard();
            void resetGame();
            void cleanup();
            void handleInput(EEvent event);
            void updateGame();
            void renderGame();
            void spawnFood();
            bool isOpposite(Direction first, Direction second) const;
            bool isOnSnake(const Cell &cell, std::size_t ignoreTail) const;
            std::size_t getIndex(const Cell &cell) const;
            void updateScoreText();
            void updateStatusText(const std::string &status);
    };
}
