/*
** EPITECH PROJECT, 2025
** G-OOP-400-LIL-4-1-arcade-9
** File description:
** Pacman.cpp
*/

#include "Pacman.hpp"
#include "Exception.hpp"

__attribute__((constructor)) void create(void)
{
    return;
}

__attribute__((destructor)) void destroy(void)
{
    return;
}

extern "C" {
    arcade::Pacman *myEntryPoint(void)
    {
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
    bool Pacman::changeDisplay()
    {
        bool change = _changeDisplay;
        _changeDisplay = false;
        return change;
    }

    void Pacman::resetGame(std::shared_ptr<IDisplayModule> display)
    {
        _map.clear();
        _gumMap.clear();
        _rectBase = nullptr;
        _rectScore = nullptr;
        _rectWord = nullptr;
        _pacman = nullptr;
        for (auto &ghost : _ghosts)
            ghost.rect = nullptr;
        map = _initialMap;
        _score = 0;
        _pacmanStartX = 13;
        _pacmanStartY = 17;
        _dirX = 1;
        _dirY = 0;
        _nextDirX = 1;
        _nextDirY = 0;
        _paused = false;
        _changeDisplay = false;
        _state = "RUNNING";
        _pacmanDirection = "./assets/pacman_right.png";
        _initGhostPosition = false;
        _superSonic = 0;
        _ghostSpeed = 200;
        _tileSize = 15;
        _lastMove = std::chrono::steady_clock::now();
        _ghostLastMove = std::chrono::steady_clock::now();
        _superSonicStart = std::chrono::steady_clock::now();
        loadMap(display);
        displayPacman(display);
        displayGhosts(display, true);
        _initialized = true;
    }

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

    void Pacman::loadMap(std::shared_ptr<IDisplayModule> display)
    {
        _windowSize = display->getWindowSize();
        if (_windowSize.w < 300)
            _tileSize = 1;
        else
            _tileSize = TILE_SIZE;
        if (_windowSize.w < MAP_WIDTH || _windowSize.h < MAP_HEIGHT) {
            throw Exception("Pacman needs at least " + std::to_string(MAP_WIDTH) + "x" +
                std::to_string(MAP_HEIGHT) + " terminal cells in ncurses");
        }
        _rectBounds = {_tileSize * MAP_WIDTH, _tileSize * MAP_HEIGHT, 0, 0};
        _rectBounds.x = (_windowSize.w - _rectBounds.w) / 2;
        _rectBounds.y = (_windowSize.h - _rectBounds.h) / 2;
        size_t scoreW = (_tileSize == 1) ? 14 : 150;
        size_t scoreH = (_tileSize == 1) ? 3 : 50;
        size_t wordW  = (_tileSize == 1) ? 14 : 180;
        size_t scoreY = (_rectBounds.y > scoreH) ? _rectBounds.y - scoreH : 0;
        Bounds scoreBounds = {scoreW, scoreH, _rectBounds.x, scoreY};
        Bounds runningBounds = {wordW, scoreH, _rectBounds.x + _rectBounds.w - wordW, scoreY};
        _rectBase = display->createRect(_rectBounds);
        _rectScore = display->createRect(scoreBounds);
        _rectWord = display->createRect(runningBounds);
        _gumMap.resize(MAP_HEIGHT, std::vector<std::shared_ptr<IRect>>(MAP_WIDTH, nullptr));
        size_t borderThick = (_tileSize > 1) ? 2 : 1;
        for (size_t i = 0; i < map.size(); i++) {
            for (size_t j = 0; j < map[i].size(); j++) {
                if (map[i][j] == '#') {
                    bool top = (i == 0 || map[i - 1][j] != '#');
                    bool bot = (i + 1 >= map.size() || map[i + 1][j] != '#');
                    bool left = (j == 0 || map[i][j - 1] != '#');
                    bool right = (j + 1 >= map[i].size() || map[i][j + 1] != '#');
                    if (top) {
                        std::shared_ptr<IRect> topBorder = display->createRect({_tileSize, borderThick, _rectBounds.x + j * _tileSize, _rectBounds.y + i * _tileSize});
                        topBorder->setTexture({"", 21, 1, 87, 255});
                        _map.push_back({topBorder, true});
                    }
                    if (bot) {
                        std::shared_ptr<IRect> bottomBorder = display->createRect({_tileSize, borderThick, _rectBounds.x + j * _tileSize, _rectBounds.y + i * _tileSize + _tileSize - borderThick});
                        bottomBorder->setTexture({"", 21, 1, 87, 255});
                        _map.push_back({bottomBorder, true});
                    }
                    if (left) {
                        std::shared_ptr<IRect> leftBorder = display->createRect({borderThick, _tileSize, _rectBounds.x + j * _tileSize, _rectBounds.y + i * _tileSize});
                        leftBorder->setTexture({"", 21, 1, 87, 255});
                        _map.push_back({leftBorder, true});
                    }
                    if (right) {
                        std::shared_ptr<IRect> rightBorder = display->createRect({borderThick, _tileSize, _rectBounds.x + j * _tileSize + _tileSize - borderThick, _rectBounds.y + i * _tileSize});
                        rightBorder->setTexture({"", 21, 1, 87, 255});
                        _map.push_back({rightBorder, true});
                    }
                }
                if (map[i][j] == '0') {
                    std::shared_ptr<IRect> bigGum = display->createRect({_tileSize, _tileSize, _rectBounds.x + j * _tileSize, _rectBounds.y + i * _tileSize});
                    bigGum->setTexture({"./assets/pacgum.png", 224, 0, 0, 255});
                    _gumMap[i][j] = bigGum;
                    _map.push_back({bigGum, true});
                }
                if (map[i][j] == '.') {
                    size_t gumSize = (_tileSize > 1) ? 6 : 1;
                    size_t gumOff  = (_tileSize > 1) ? 4 : 0;
                    std::shared_ptr<IRect> gum = display->createRect({gumSize, gumSize, _rectBounds.x + j * _tileSize + gumOff, _rectBounds.y + i * _tileSize + gumOff});
                    gum->setTexture({"./assets/pacgum.png", 15, 224, 0, 255});
                    _gumMap[i][j] = gum;
                    _map.push_back({gum, true});
                }
            }
        }
        display->setBackground({"./assets/pacman_background.jpg", 0, 0, 0, 255});
        _rectBase->setTexture({"", 0, 0, 0, 0});
        _rectScore->setTexture({"", 0, 0, 0, 0});
        _rectWord->setTexture({"", 0, 0, 0, 0});
        std::string scoreValue = std::to_string(_score);
        _rectScore->setText(std::string ("score: ") + scoreValue, {"./assets/font/font.ttf", 255, 255, 255, 255});
        _rectWord->setText(_state, {"./assets/font/font.ttf", 255, 255, 255, 255});
    }

    void Pacman::displayPacman(std::shared_ptr<IDisplayModule> display)
    {
        _pacman = display->createRect({_tileSize, _tileSize,
            _rectBounds.x + _pacmanStartX * _tileSize,
            _rectBounds.y + _pacmanStartY * _tileSize});
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
        _pacman->setPosition({_rectBounds.x + _pacmanStartX * _tileSize, _rectBounds.y + _pacmanStartY * _tileSize});
        if (map[_pacmanStartY][_pacmanStartX] == '.') {
            std::shared_ptr<IRect> gum = _gumMap[_pacmanStartY][_pacmanStartX];
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
            std::shared_ptr<IRect> gum = _gumMap[_pacmanStartY][_pacmanStartX];
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
        _pacman->setTexture({_pacmanDirection, 224, 213, 0, 255});
    }

    void Pacman::run(std::shared_ptr<IDisplayModule> display, const std::string name)
    {
        bool running = true;
        _changeDisplay = false;
        if (!_initialized) {
            resetGame(display);
        } else {
            _map.clear();
            _gumMap.clear();
            _rectBase = nullptr;
            _rectScore = nullptr;
            _rectWord = nullptr;
            _pacman = nullptr;
            for (auto &ghost : _ghosts)
                ghost.rect = nullptr;
            loadMap(display);
            displayPacman(display);
            displayGhosts(display, false);
        }
        while (running) {
            auto now = std::chrono::steady_clock::now();
            EEvent event = display->pollEvent();
            if (event == QUIT || event == ESCAPE)
                running = false;
            if (event == TAB) {
                _changeDisplay = true;
                running = false;
                continue;
            }
            if (event == ENTER && (_state == "GAME OVER" || _state == "VICTORY")) {
                resetGame(display);
                continue;
            }
            updateDirection(event);
            if (!_paused) {
                changePacman();
                movePacman();
                if (std::chrono::duration_cast<std::chrono::seconds>(now - _ghostCageStart).count() >= 10)
                    moveGhosts();
            }
            std::string scoreValue = std::to_string(_score);
            _rectScore->setText("score: " + scoreValue, {"./assets/font/font.ttf", 255, 255, 255, 255});
            _rectWord->setText(_state, {"./assets/font/font.ttf", 255, 255, 255, 255});
            display->clearWindow();
            _rectBase->display();
            for (auto &mapRect : _map)
                if (mapRect.visible)
                    mapRect.rect->display();
            _rectScore->display();
            _rectWord->display();
            display->render();
        }
        std::ofstream file("scores.txt", std::ios::app);
        if (file) {
            file << name << " Pacman " << _score << std::endl;
            file.close();
        }
    }

    void Pacman::displayGhosts(std::shared_ptr<IDisplayModule> display, bool resetState)
    {
        std::array<std::string, 4> textures = {"./assets/red_ghost.png", "./assets/pink_ghost.png", "./assets/blue_ghost.png", "./assets/yellow_ghost.png"};
        auto now = std::chrono::steady_clock::now();

        for (int i = 0; i < 4; i++) {
            if (resetState) {
                _ghosts[i].x = _startPositions[i].first;
                _ghosts[i].y = _startPositions[i].second;
                _ghosts[i].inCage = true;
                _ghosts[i].cageReleaseTime = now + std::chrono::seconds(10);
                _ghosts[i].texture = textures[i];
                _ghosts[i].dirX = 0;
                _ghosts[i].dirY = -1;
            } else if (_ghosts[i].texture.empty())
                _ghosts[i].texture = textures[i];
            _ghosts[i].rect = display->createRect({_tileSize, _tileSize, _rectBounds.x + _ghosts[i].x * _tileSize, _rectBounds.y + _ghosts[i].y * _tileSize});
            if (_ghosts[i].rect) {
                if (_superSonic)
                    _ghosts[i].rect->setTexture({"./assets/blue_eat_pacman.png", 1, 120, 181, 255});
                else
                    _ghosts[i].rect->setTexture({_ghosts[i].texture, 255, 255, 255, 255});
                _map.push_back({_ghosts[i].rect, true});
            }
        }
        if (resetState)
            _ghostCageStart = now;
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

    void Pacman::moveGhosts()
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
                _ghosts[i].cageReleaseTime = now;
            }
            _initGhostPosition = true;
        }
        const std::array<std::pair<int,int>, 4> dirs = {{{0, -1}, {1, 0}, {-1, 0}, {0, 1}}};
        for (size_t i = 0; i < _ghosts.size(); i++) {
            Ghost &ghost = _ghosts[i];
            if (ghost.inCage && now < ghost.cageReleaseTime) {
                ghost.rect->setPosition({
                    _rectBounds.x + ghost.x * _tileSize,
                    _rectBounds.y + ghost.y * _tileSize
                });
                continue;
            }
            if (ghost.inCage && now >= ghost.cageReleaseTime) {
                ghost.x = movePositions[i].first;
                ghost.y = movePositions[i].second;
                ghost.inCage = false;
                ghost.dirX = 0;
                ghost.dirY = -1;
            }

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
                _rectBounds.x + ghost.x * _tileSize,
                _rectBounds.y + ghost.y * _tileSize
            });
            if (_superSonic) {
                ghost.rect->setTexture({"./assets/blue_eat_pacman.png", 0, 120, 181, 255});
                if (std::chrono::duration_cast<std::chrono::seconds>(now - _superSonicStart).count() >= 10) {
                    std::array<std::string, 4> textures = {"./assets/red_ghost.png", "./assets/pink_ghost.png", "./assets/blue_ghost.png", "./assets/yellow_ghost.png"};
                    for (int i = 0; i < 4; i++)
                        _ghosts[i].rect->setTexture({textures[i], 255, 255, 255, 255});
                    _superSonic = 0;
                }
                if (ghost.x == (int)_pacmanStartX && ghost.y == (int)_pacmanStartY) {
                    _score += 200 * _superSonic;
                    _superSonic++;
                    ghost.x = _startPositions[i].first;
                    ghost.y = _startPositions[i].second;
                    ghost.inCage = true;
                    ghost.cageReleaseTime = now + std::chrono::seconds(5);
                    ghost.dirX = 0;
                    ghost.dirY = -1;
                    ghost.rect->setTexture({ghost.texture, 255, 255, 255, 0});
                    ghost.rect->setPosition({
                        _rectBounds.x + ghost.x * _tileSize,
                        _rectBounds.y + ghost.y * _tileSize
                    });
                    continue;
                }
            }
            if (ghost.x == (int)_pacmanStartX && ghost.y == (int)_pacmanStartY) {
                _state = "GAME OVER";
                _paused = true;
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
}
