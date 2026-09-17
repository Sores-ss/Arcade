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
        private:
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
            std::vector<Size> _snake;
            Size _food = {0, 0};
            Direction _direction = DIR_RIGHT;
            Direction _nextDirection = DIR_RIGHT;
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
            bool isOnSnake(const Size &cell, std::size_t ignoreTail) const;
            std::size_t getIndex(const Size &cell) const;
            void updateScoreText();
            void updateStatusText(const std::string &status);
    };
}
