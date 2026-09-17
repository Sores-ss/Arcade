/*
** EPITECH PROJECT, 2025
** G-OOP-400-LIL-4-1-arcade-9
** File description:
** Pacman.cpp
*/

#include "Pacman.hpp"

__attribute__((constructor)) void create(void)
{
    std::cout << "Opening pacman..." << std::endl;
}

__attribute__((destructor)) void destroy(void)
{
    std::cout << "Closing pacman..." << std::endl;
}

extern "C" {
    arcade::Pacman *myEntryPoint(void)
    {
        std::cout << "Loading pacman.." << std::endl;
        return new arcade::Pacman();
    }

    arcade::EType getLibType(void)
    {
        return arcade::EType::GAME;
    }

    std::string getLibName(void)
    {
        return "Pacman";
    }
}

namespace arcade {
    void Pacman::loadMap(IDisplayModule *display)
    {
        _windowSize = display->getWindowSize();
        _rectBounds = {455, 500, 0, 0};
        _rectBounds.x = (_windowSize.w - _rectBounds.w) / 2 + 20;
        _rectBounds.y = (_windowSize.h - _rectBounds.h) / 2 - 50;
        Bounds scoreBounds = {150, 50, _rectBounds.x, 0};
        Bounds runningBounds = {180, 50, _rectBounds.x + _rectBounds.w - 180, 0};
        scoreBounds.y = (_rectBounds.y > scoreBounds.h + 20) ? _rectBounds.y - scoreBounds.h - 20 : 20;
        runningBounds.y = scoreBounds.y;
        _rectBase = display->createRect(_rectBounds);
        _rectScore = display->createRect(scoreBounds);
        _rectWord = display->createRect(runningBounds);
        _gumMap.resize(MAP_HEIGHT, std::vector<IRect *>(MAP_WIDTH, nullptr));
        for (size_t i = 0; i < map.size(); i++) {
            for (size_t j = 0; j < map[i].size(); j++) {
                if (map[i][j] == '#') {
                    bool top = (i == 0 || map[i - 1][j] != '#');
                    bool bot = (i + 1 >= map.size() || map[i + 1][j] != '#');
                    bool left = (j == 0 || map[i][j - 1] != '#');
                    bool right = (j + 1 >= map[i].size() || map[i][j + 1] != '#');
                    if (top) {
                        IRect *topBorder = display->createRect({15, 2, _rectBounds.x + j * 15, _rectBounds.y + i * 15});
                        topBorder->setTexture({"", 21, 1, 87, 0});
                        _map.push_back({topBorder, true});
                    }
                    if (bot) {
                        IRect *bottomBorder = display->createRect({15, 2, _rectBounds.x + j * 15, _rectBounds.y + i * 15 + 15 - 2});
                        bottomBorder->setTexture({"", 21, 1, 87, 0});
                        _map.push_back({bottomBorder, true});
                    }
                    if (left) {
                        IRect *leftBorder = display->createRect({2, 15, _rectBounds.x + j * 15, _rectBounds.y + i * 15});
                        leftBorder->setTexture({"", 21, 1, 87, 0});
                        _map.push_back({leftBorder, true});
                    }
                    if (right) {
                        IRect *rightBorder = display->createRect({2, 15, _rectBounds.x + j * 15 + 15 - 2, _rectBounds.y + i * 15});
                        rightBorder->setTexture({"", 21, 1, 87, 0});
                        _map.push_back({rightBorder, true});
                    }
                }
                if (map[i][j] == '0') {
                    IRect *bigGum = display->createRect({15, 15, _rectBounds.x + j * 15, _rectBounds.y + i * 15});
                    bigGum->setTexture({"./assets/pacgum.png", 224, 213, 0, 0});
                    _gumMap[i][j] = bigGum;
                    _map.push_back({bigGum, true});
                }
                if (map[i][j] == '.') {
                    IRect *gum = display->createRect({6, 6, _rectBounds.x + j * 15 + 4, _rectBounds.y + i * 15 + 4});
                    gum->setTexture({"./assets/pacgum.png", 224, 213, 0, 0});
                    _gumMap[i][j] = gum;
                    _map.push_back({gum, true});
                }
            }
        }
        display->setBackground({"./assets/pacman_background.jpg", 0, 0, 0, 0});
        _rectBase->setTexture({"", 0, 0, 0, 0});
        _rectScore->setTexture({"", 0, 0, 0, 0});
        _rectWord->setTexture({"", 0, 0, 0, 0});
        std::string scoreValue = std::to_string(_score);
        _rectScore->setText(std::string ("score: ") + scoreValue, {"./assets/Pixellettersfull-BnJ5.ttf", 255, 255, 255, 0});
        _rectWord->setText(_state, {"./assets/Pixellettersfull-BnJ5.ttf", 255, 255, 255, 0});
    }

    void Pacman::displayPacman(IDisplayModule *display)
    {
        _pacman = display->createRect({TILE_SIZE, TILE_SIZE,
            _rectBounds.x + _pacmanStartX * TILE_SIZE,
            _rectBounds.y + _pacmanStartY * TILE_SIZE});
        if (!_pacman)
            return;
        _lastMove = std::chrono::steady_clock::now();
        _pacman->setTexture({_pacmanDirection, 240, 228, 0, 0});
        _map.push_back({_pacman, true});
    }

    void Pacman::updateDirection(EEvent event)
    {
        if (event == EEvent::UP) {
            _dirX = 0;
            _dirY = -1;
        }
        if (event == EEvent::DOWN) {
            _dirX = 0;
            _dirY = 1;
        }
        if (event == EEvent::LEFT) {
            _dirX = -1;
            _dirY = 0;
        }
        if (event == EEvent::RIGHT) {
            _dirX = 1;
            _dirY = 0;
        }
        if (event == ENTER)
            pause();
    }

    void Pacman::movePacman()
    {
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - _lastMove);
        int nextX = _pacmanStartX + _dirX;
        int nextY = _pacmanStartY + _dirY;

        if (elapsed.count() < 120)
            return;
        _lastMove = now;
        if (nextX < 0 || nextY < 0 || nextX >= MAP_WIDTH || nextY >= MAP_HEIGHT)
            return;
        if (map[nextY][nextX] != '#') {
            _pacmanStartX = nextX;
            _pacmanStartY = nextY;
        }
        _pacman->setPosition({_rectBounds.x + _pacmanStartX * TILE_SIZE, _rectBounds.y + _pacmanStartY * TILE_SIZE});
        if (map[_pacmanStartY][_pacmanStartX] == '.') {
            IRect *gum = _gumMap[_pacmanStartY][_pacmanStartX];
            if (gum) {
                for (auto &tile : _map)
                    if (tile.rect == gum){
                        tile.visible = false;
                        break;
                    }
            }
            _gumMap[_pacmanStartY][_pacmanStartX] = nullptr;
            map[_pacmanStartY][_pacmanStartX] = ' ';
            _score += 10;
        }
    }

    void Pacman::changePacman()
    {
        if (_dirX == 1)
            _pacmanDirection = "./assets/pacman_right.png";
        if (_dirX == -1)
            _pacmanDirection = "./assets/pacman_left.png";
        if (_dirY == 1)
            _pacmanDirection = "./assets/pacman_bot.png";
        if (_dirY == -1)
            _pacmanDirection = "./assets/pacman_top.png";
        _pacman->setTexture({_pacmanDirection, 240, 228, 0, 0});
    }

    void Pacman::run(IDisplayModule *display)
    {
        bool running = true;

        loadMap(display);
        displayPacman(display);
        while (running) {
            EEvent event = display->pollEvent();
            if (event == EEvent::QUIT || event == EEvent::ESCAPE)
                running = false;
            updateDirection(event);
            if (!_paused) {
                changePacman();
                movePacman();
            }
            std::string scoreValue = std::to_string(_score);
            _rectScore->setText("score: " + scoreValue, {"./assets/Pixellettersfull-BnJ5.ttf", 255, 255, 255, 0});
            _rectWord->setText(_state, {"./assets/Pixellettersfull-BnJ5.ttf", 255, 255, 255, 0});
            display->clearWindow();
            _rectBase->display();
            for (auto &mapRect : _map)
                if (mapRect.visible)
                    mapRect.rect->display();
            _rectScore->display();
            _rectWord->display();
            display->render();
        }
    }

    void Pacman::pause()
    {
        _paused = !_paused;
        if (_paused)
            _state = "PAUSED";
        else
            _state = "RUNNING";
        std::cout << "pacman paused" << std::endl;
    }

    void Pacman::stop()
    {
        std::cout << "pacman stopped" << std::endl;
    }
}
