/*
** EPITECH PROJECT, 2025
** G-OOP-400-LIL-4-1-arcade-9
** File description:
** Snake.cpp
*/

#include "Snake.hpp"
#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <thread>

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

    arcade::EType getLibType(void)
    {
        return arcade::EType::GAME;
    }

    const std::string getLibName(void)
    {
        return "Snake";
    }
}

namespace arcade {
    static const Texture LIGHT_TILE = {"", 45, 145, 50, 255};
    static const Texture DARK_TILE = {"", 30, 120, 40, 255};
    static const Texture BODY_TILE = {"", 245, 230, 130, 255};
    static const Texture HEAD_TILE = {"", 255, 180, 70, 255};
    static const Texture FOOD_TILE = {"", 210, 40, 40, 255};
    static const Texture PANEL_TILE = {"", 35, 35, 35, 220};
    static const Texture TEXT_TILE = {"./assets/Pixellettersfull-BnJ5.ttf", 255, 255, 255, 255};

    bool Snake::isOpposite(Direction first, Direction second) const
    {
        if (first == DIR_UP && second == DIR_DOWN)
            return true;
        if (first == DIR_DOWN && second == DIR_UP)
            return true;
        if (first == DIR_LEFT && second == DIR_RIGHT)
            return true;
        if (first == DIR_RIGHT && second == DIR_LEFT)
            return true;
        return false;
    }

    std::size_t Snake::getIndex(const Size &cell) const
    {
        return cell.h * _mapWidth + cell.w;
    }

    bool Snake::isOnSnake(const Size &cell, std::size_t ignoreTail) const
    {
        if (_snake.empty())
            return false;
        std::size_t limit = _snake.size();
        if (ignoreTail > limit)
            ignoreTail = limit;
        limit -= ignoreTail;
        for (std::size_t i = 0; i < limit; ++i) {
            if (_snake[i].w == cell.w && _snake[i].h == cell.h)
                return true;
        }
        return false;
    }

    void Snake::spawnFood()
    {
        std::vector<Size> freeCells;

        for (std::size_t y = 0; y < _mapHeight; ++y) {
            for (std::size_t x = 0; x < _mapWidth; ++x) {
                Size cell = {x, y};
                if (!isOnSnake(cell, 0))
                    freeCells.push_back(cell);
            }
        }
        if (freeCells.empty()) {
            _gameOver = true;
            updateStatusText("YOU WIN - ENTER");
            return;
        }
        std::size_t pick = static_cast<std::size_t>(std::rand()) % freeCells.size();
        _food = freeCells[pick];
    }

    void Snake::updateScoreText()
    {
        if (_scoreRect == nullptr || _display == nullptr)
            return;
        _scoreRect->setText("SCORE: " + std::to_string(_score), TEXT_TILE);
    }

    void Snake::updateStatusText(const std::string &status)
    {
        if (_statusRect == nullptr || _display == nullptr)
            return;
        _statusRect->setText(status, TEXT_TILE);
    }

    void Snake::initBoard()
    {
        Size windowSize = _display->getWindowSize();
        std::size_t gridWidth = _mapWidth * _cellSize;
        std::size_t gridHeight = _mapHeight * _cellSize;
        std::size_t mapX = (windowSize.w - gridWidth) / 2;
        std::size_t mapY = (windowSize.h - gridHeight) / 2;

        for (std::size_t y = 0; y < _mapHeight; ++y) {
            for (std::size_t x = 0; x < _mapWidth; ++x) {
                IRect *tile = _display->createRect({_cellSize, _cellSize,
                    mapX + x * _cellSize, mapY + y * _cellSize});
                _tiles.push_back(tile);
            }
        }
        _scoreRect = _display->createRect({260, 65, mapX, mapY - 90});
        _statusRect = _display->createRect({360, 65, mapX + gridWidth - 360, mapY - 90});
        _scoreRect->setTexture(PANEL_TILE);
        _statusRect->setTexture(PANEL_TILE);
        updateStatusText("RUNNING");
    }

    void Snake::resetGame()
    {
        std::size_t startX = _mapWidth / 2;
        std::size_t startY = _mapHeight / 2;

        _snake.clear();
        _snake.push_back({startX, startY});
        _snake.push_back({startX - 1, startY});
        _snake.push_back({startX - 2, startY});
        _direction = DIR_RIGHT;
        _nextDirection = DIR_RIGHT;
        _paused = false;
        _gameOver = false;
        _score = 0;
        spawnFood();
        updateScoreText();
        updateStatusText("RUNNING");
    }

    void Snake::cleanup()
    {
        for (IRect *tile : _tiles)
            delete tile;
        _tiles.clear();
        if (_scoreRect != nullptr)
            delete _scoreRect;
        if (_statusRect != nullptr)
            delete _statusRect;
        _scoreRect = nullptr;
        _statusRect = nullptr;
        _snake.clear();
    }

    void Snake::handleInput(EEvent event)
    {
        Direction wanted = _nextDirection;

        if (event == EEvent::QUIT || event == EEvent::ESCAPE) {
            _running = false;
            return;
        }
        if (_gameOver) {
            if (event == EEvent::ENTER)
                resetGame();
            return;
        }
        if (event == EEvent::ENTER) {
            pause();
            return;
        }
        if (_paused)
            return;
        if (event == EEvent::UP)
            wanted = DIR_UP;
        if (event == EEvent::DOWN)
            wanted = DIR_DOWN;
        if (event == EEvent::LEFT)
            wanted = DIR_LEFT;
        if (event == EEvent::RIGHT)
            wanted = DIR_RIGHT;
        if (!isOpposite(_direction, wanted))
            _nextDirection = wanted;
    }

    void Snake::updateGame()
    {
        Size nextHead = _snake.front();
        bool grows = false;

        _direction = _nextDirection;
        if (_direction == DIR_UP) {
            if (nextHead.h == 0) {
                _gameOver = true;
                updateStatusText("GAME OVER - ENTER");
                return;
            }
            nextHead.h--;
        }
        if (_direction == DIR_DOWN)
            nextHead.h++;
        if (_direction == DIR_LEFT) {
            if (nextHead.w == 0) {
                _gameOver = true;
                updateStatusText("GAME OVER - ENTER");
                return;
            }
            nextHead.w--;
        }
        if (_direction == DIR_RIGHT)
            nextHead.w++;
        if (nextHead.w >= _mapWidth || nextHead.h >= _mapHeight) {
            _gameOver = true;
            updateStatusText("GAME OVER - ENTER");
            return;
        }
        if (nextHead.w == _food.w && nextHead.h == _food.h)
            grows = true;
        if (isOnSnake(nextHead, grows ? 0 : 1)) {
            _gameOver = true;
            updateStatusText("GAME OVER - ENTER");
            return;
        }
        _snake.insert(_snake.begin(), nextHead);
        if (grows) {
            _score++;
            updateScoreText();
            spawnFood();
            return;
        }
        _snake.pop_back();
    }

    void Snake::renderGame()
    {
        for (std::size_t y = 0; y < _mapHeight; ++y) {
            for (std::size_t x = 0; x < _mapWidth; ++x) {
                Texture checker = ((x + y) % 2 == 0) ? LIGHT_TILE : DARK_TILE;
                _tiles[y * _mapWidth + x]->setTexture(checker);
            }
        }
        for (std::size_t i = 1; i < _snake.size(); ++i)
            _tiles[getIndex(_snake[i])]->setTexture(BODY_TILE);
        _tiles[getIndex(_snake.front())]->setTexture(HEAD_TILE);
        _tiles[getIndex(_food)]->setTexture(FOOD_TILE);
        _scoreRect->display();
        _statusRect->display();
        for (IRect *tile : _tiles)
            tile->display();
    }

    void Snake::run(IDisplayModule *display)
    {
        if (display == nullptr)
            return;
        _display = display;
        std::srand(std::time(nullptr));
        _running = true;
        initBoard();
        resetGame();

        auto lastTick = std::chrono::steady_clock::now();
        const std::chrono::milliseconds tickRate(120);

        while (_running) {
            EEvent event = _display->pollEvent();
            auto now = std::chrono::steady_clock::now();

            handleInput(event);
            if (!_paused && !_gameOver && now - lastTick >= tickRate) {
                updateGame();
                lastTick = now;
            }
            _display->clearWindow();
            renderGame();
            _display->render();
            std::this_thread::sleep_for(std::chrono::milliseconds(2));
        }
        cleanup();
        _display = nullptr;
    }

    void Snake::pause()
    {
        _paused = !_paused;
        if (_paused)
            updateStatusText("PAUSED");
        else
            updateStatusText("RUNNING");
    }

    void Snake::stop()
    {
        _running = false;
    }
}
