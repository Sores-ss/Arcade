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
    int Pacman::handleX(int x) const
    {
        if (x < 1)
            return MAP_WIDTH - 2;
        if (x >= MAP_WIDTH - 1)
            return 1;
        return x;
    }

    int Pacman::handleY(int y) const
    {
        if (y < 1)
            return MAP_HEIGHT - 2;
        if (y >= MAP_HEIGHT - 1)
            return 1;
        return y;
    }

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
        if (event == UP) {
            _nextDirX = 0;
            _nextDirY = -1;
        }
        if (event == DOWN) {
            _nextDirX = 0;
            _nextDirY = 1;
        }
        if (event == LEFT) {
            _nextDirX = -1;
            _nextDirY = 0;
        }
        if (event == RIGHT) {
            _nextDirX = 1;
            _nextDirY = 0;
        }
        if (event == ENTER)
            pause();
    }

    void Pacman::movePacman()
    {
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - _lastMove);

        if (elapsed.count() < 120)
            return;
        _lastMove = now;
        int wantedX = handleX(_pacmanStartX + _nextDirX);
        int wantedY = handleY(_pacmanStartY + _nextDirY);
        if (map[wantedY][wantedX] != '#') {
            _dirX = _nextDirX;
            _dirY = _nextDirY;
        }
        int nextX = handleX(_pacmanStartX + _dirX);
        int nextY = handleY(_pacmanStartY + _dirY);
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
            bool hasRemainingGums = false;
            for (const auto &row : map) {
                for (char tile : row) {
                    if (tile == '.' || tile == '0') {
                        hasRemainingGums = true;
                        break;
                    }
                }
                if (hasRemainingGums)
                    break;
            }
            if (!hasRemainingGums) {
                _state = "VICTORY";
                _paused = true;
            }
        }
        if (map[_pacmanStartY][_pacmanStartX] == '0') {
            IRect *gum = _gumMap[_pacmanStartY][_pacmanStartX];
            if (gum) {
                for (auto &tile : _map) {
                    if (tile.rect == gum) {
                        tile.visible = false;
                        break;
                    }
                }
            }
            _gumMap[_pacmanStartY][_pacmanStartX] = nullptr;
            map[_pacmanStartY][_pacmanStartX] = ' ';
            _superSonic = 1;
            _ghostSpeed = 250;
            _superSonicStart = std::chrono::steady_clock::now();
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
        auto start = std::chrono::steady_clock::now();
        auto last = start;

        _running = true;
        loadMap(display);
        displayPacman(display);
        displayGhosts(display);
        while (_running) {
            auto now = std::chrono::steady_clock::now();
            EEvent event = display->pollEvent();
            if (event == QUIT || event == ESCAPE)
                _running = false;
            updateDirection(event);
            if (!_paused) {
                changePacman();
                movePacman();
                if (std::chrono::duration_cast<std::chrono::seconds>(now - last).count() >= 10)
                    moveGhosts(display);
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

    void Pacman::displayGhosts(IDisplayModule *display)
    {
        std::array<std::pair<int,int>, 4> startPositions = {{{12, 14}, {13, 14}, {14, 14}, {15, 14}}};
        std::array<std::string, 4> textures = {"./assets/red_ghost.png", "./assets/pink_ghost.png", "./assets/blue_ghost.png", "./assets/yellow_ghost.png"};

        for (int i = 0; i < 4; i++) {
            _ghosts[i].x = startPositions[i].first;
            _ghosts[i].y = startPositions[i].second;
            _ghosts[i].inCage = true;
            _ghosts[i].texture = textures[i];
            _ghosts[i].dirX = 0;
            _ghosts[i].dirY = -1;
            _ghosts[i].rect = display->createRect({TILE_SIZE, TILE_SIZE, _rectBounds.x + _ghosts[i].x * TILE_SIZE, _rectBounds.y + _ghosts[i].y * TILE_SIZE});
            if (_ghosts[i].rect) {
                _ghosts[i].rect->setTexture({textures[i], 255, 255, 255, 0});
                _map.push_back({_ghosts[i].rect, true});
            }
        }
        _ghostCageStart = std::chrono::steady_clock::now();
    }

    bool Pacman::isWalkableTile(int x, int y) const
    {
        if (x < 0 || y < 0 || x >= MAP_WIDTH || y >= MAP_HEIGHT)
            return false;
        return map[y][x] != '#';
    }

    bool Pacman::isInCage(int x, int y) const
    {
        return (x >= _cageLeft && x <= _cageRight && y >= _cageTop && y <= _cageBottom);
    }

    bool Pacman::canGhostMoveTo(size_t ghostIndex, int x, int y) const
    {
        if (!isWalkableTile(x, y))
            return false;
        if (_ghosts[ghostIndex].inCage) {
            bool isCageTile = isInCage(x, y);
            bool isExitTile = (y == 12 && x >= 13 && x <= 14);
            if (!isCageTile && !isExitTile)
                return false;
        }
        return true;
    }

    void Pacman::moveGhosts(IDisplayModule *display)
    {
        auto now = std::chrono::steady_clock::now();
        auto elapsedMove = std::chrono::duration_cast<std::chrono::milliseconds>(now - _ghostLastMove);
        std::array<std::pair<int,int>, 4> movePositions = {{{14, 12}, {10, 15}, {18, 14}, {14, 18}}};

        if (elapsedMove.count() < _ghostSpeed)
            return;
        _ghostLastMove = now;
        if (!_initGhostPosition) {
            for (int i = 0; i < 4; i++) {
                _ghosts[i].x = movePositions[i].first;
                _ghosts[i].y = movePositions[i].second;
                _ghosts[i].inCage = false;
            }
            _initGhostPosition = true;
        }
        const std::array<std::pair<int,int>, 4> dirs = {{{0, -1}, {1, 0}, {-1, 0}, {0, 1}}};
        for (size_t i = 0; i < _ghosts.size(); i++) {
            Ghost &ghost = _ghosts[i];
            int nextX = ghost.x + ghost.dirX;
            int nextY = ghost.y + ghost.dirY;
            if (canGhostMoveTo(i, nextX, nextY)) {
                if (!ghost.inCage && std::rand() % 10 == 0) {
                    std::vector<std::pair<int,int>> validDirs;
                    for (auto &dir : dirs) {
                        if (dir.first == -ghost.dirX && dir.second == -ghost.dirY)
                            continue;
                        if (canGhostMoveTo(i, ghost.x + dir.first, ghost.y + dir.second))
                            validDirs.push_back(dir);
                    }
                    if (!validDirs.empty()) {
                        auto chosen = validDirs[std::rand() % validDirs.size()];
                        ghost.dirX = chosen.first;
                        ghost.dirY = chosen.second;
                        nextX = ghost.x + ghost.dirX;
                        nextY = ghost.y + ghost.dirY;
                    }
                }
                ghost.x = nextX;
                ghost.y = nextY;
            } else {
                std::vector<std::pair<int,int>> validDirs;
                for (auto &dir : dirs) {
                    if (dir.first == -ghost.dirX && dir.second == -ghost.dirY)
                        continue;
                    if (canGhostMoveTo(i, ghost.x + dir.first, ghost.y + dir.second))
                        validDirs.push_back(dir);
                }
                if (!validDirs.empty()) {
                    auto chosen = validDirs[std::rand() % validDirs.size()];
                    ghost.dirX = chosen.first;
                    ghost.dirY = chosen.second;
                    ghost.x += ghost.dirX;
                    ghost.y += ghost.dirY;
                }
            }
            ghost.rect->setPosition({
                _rectBounds.x + ghost.x * TILE_SIZE,
                _rectBounds.y + ghost.y * TILE_SIZE
            });
            if (_superSonic) {
                ghost.rect->setTexture({"./assets/blue_eat_pacman.png", 255, 255, 255, 0});
                auto now = std::chrono::steady_clock::now();
                if (std::chrono::duration_cast<std::chrono::seconds>(now - _superSonicStart).count() >= 10) {
                    std::array<std::string, 4> textures = {"./assets/red_ghost.png", "./assets/pink_ghost.png", "./assets/blue_ghost.png", "./assets/yellow_ghost.png"};
                    for (int i = 0; i < 4; i++)
                        _ghosts[i].rect->setTexture({textures[i], 255, 255, 255, 0});
                    _superSonic = 0;
                }
                if (ghost.x == (int)_pacmanStartX && ghost.y == (int)_pacmanStartY) {
                    _score += 200 * _superSonic;
                    _superSonic++;
                    continue;
                }
            }
            if (ghost.x == (int)_pacmanStartX && ghost.y == (int)_pacmanStartY) {
                _state = "GAME OVER";
                _paused = true;
                EEvent event = display->pollEvent();
                if (event == ENTER)
                    run(display);
            }
        }
    }

    void Pacman::pause()
    {
        _paused = !_paused;
        if (_paused)
            _state = "PAUSED";
        else
            _state = "RUNNING";
    }

    void Pacman::stop()
    {
        _running = false;
        std::cout << "Pacman stopped" << std::endl;
    }
}
