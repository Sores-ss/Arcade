/*
** EPITECH PROJECT, 2025
** G-OOP-400-LIL-4-1-arcade-9
** File description:
** core.cpp
*/

#include "Core.hpp"
#include "Exception.hpp"
#include "DLLoader.hpp"
#include <vector>
#include <filesystem>
#include <algorithm>
#include <fstream>

namespace arcade {
    char Core::eventToChar(EEvent event)
    {
        if (event >= EEvent::A && event <= EEvent::Z)
            return (char)('A' + (event - EEvent::A));
        return '\0';
    }

    std::vector<std::string> Core::initLibList(std::string lib)
    {
        std::vector<std::string> newMap = std::vector<std::string>();
        if (!std::filesystem::exists(lib))
            throw Exception("Error: '" + lib + "' not a graphical library");
        for (const auto& entry : std::filesystem::directory_iterator("./lib")) {
            if (entry.is_regular_file()) {
                if (entry.path().extension() == ".so")
                    newMap.push_back(entry.path());
            }
        }
        return newMap;
    }

    std::string Core::loadGame(const std::string &path)
    {
        if (_game == nullptr || _currentGamePath != path) {
            _handleGame = _loaderGame.open(path);
            EType type = _loaderGame.getType(_handleGame);
            if (type != EType::GAME)
                throw Exception("Error: invalid game library : " + path);
            _game = std::unique_ptr<IGameModule>(_loaderGame.getInstance(_handleGame));
            _currentGamePath = path;
        }
        _game->run(_display, _name);
        bool wantsExit = _game->exit();
        bool wantsDisplayChange = _game->changeDisplay();
        bool wantsGameChange = _game->changeGame();
        if (wantsExit) {
            _game.reset();
            _loaderGame.close(_handleGame);
            _handleGame = nullptr;
            _currentGamePath.clear();
            return CORE_EXIT;
        }
        if (wantsDisplayChange) {
            std::string nextGraphic = changeNewDisplay();
            return nextGraphic;
        }
        if (wantsGameChange) {
            std::string nextGame = changeNewGame();
            _game.reset();
            _loaderGame.close(_handleGame);
            _handleGame = nullptr;
            _currentGamePath.clear();
            return nextGame;
        }
        _game.reset();
        _loaderGame.close(_handleGame);
        _handleGame = nullptr;
        _currentGamePath.clear();
        return "";
    }

    std::string Core::changeNewDisplay()
    {
        std::vector<std::string> graphicalLibs;

        for (const auto &lib : _libs) {
            void *handle = _loaderGraphic.open(lib);
            if (_loaderGraphic.getType(handle) == EType::GRAPHICAL)
                graphicalLibs.push_back(lib);
            _loaderGraphic.close(handle);
        }
        if (graphicalLibs.size() < 2)
            return "";
        auto current = std::find(graphicalLibs.begin(), graphicalLibs.end(), _currentGraphicPath);
        if (current == graphicalLibs.end())
            return graphicalLibs.front();
        std::size_t nextIndex = ((std::size_t)(std::distance(graphicalLibs.begin(), current)) + 1) % graphicalLibs.size();
        return graphicalLibs[nextIndex];
    }

    std::string Core::changeNewGame()
    {
        std::vector<std::string> gameLibs;

        for (const auto &lib : _libs) {
            void *handle = _loaderGame.open(lib);
            if (_loaderGame.getType(handle) == EType::GAME)
                gameLibs.push_back(lib);
            _loaderGame.close(handle);
        }
        if (gameLibs.size() < 2)
            return "";
        auto current = std::find(gameLibs.begin(), gameLibs.end(), _currentGamePath);
        if (current == gameLibs.end())
            return gameLibs.front();
        std::size_t nextIndex = ((std::size_t)(std::distance(gameLibs.begin(), current)) + 1) % gameLibs.size();
        return gameLibs[nextIndex];
    }

    std::string Core::runlib(IRect &selector, bool &running)
    {
        std::vector<std::shared_ptr<IRect>> libList;
        std::vector<void *> handles;
        Size window = _display->getWindowSize();
        size_t buttonW = std::max<size_t>(18, window.w / 3);
        size_t buttonH = std::max<size_t>(3, window.h / 10);
        size_t centerX = window.w / 2;
        size_t centerY = window.h / 2;
        size_t baseX = (centerX > (buttonW / 2)) ? centerX - (buttonW / 2) : 0;
        size_t firstY = (centerY > (buttonH * 2)) ? centerY - (buttonH * 2) : 0;
        size_t stepY = std::max<size_t>(2, buttonH + 1);
        size_t selectorW = std::max<size_t>(2, buttonW / 8);
        size_t selectorX = (baseX > selectorW + 1) ? baseX - (selectorW + 1) : 0;
        std::vector<std::string> graphicalLibs;
        std::string selectedLib;

        for (const auto &lib : _libs) {
            void *handle = _loaderGraphic.open(lib);
            if (_loaderGraphic.getType(handle) == EType::GRAPHICAL) {
                graphicalLibs.push_back(lib);
                handles.push_back(handle);
                size_t y = firstY + static_cast<size_t>(libList.size()) * stepY;
                std::shared_ptr<IRect> libRect = _display->createRect({{buttonW, buttonH}, {baseX, y}});
                libRect->setTexture({"./assets/button_texture.png", 180, 180, 180, 255});
                libRect->setText(_loaderGraphic.getName(handle), {"./assets/font/font.ttf", 255, 255, 255, 255});
                libRect->setPosition({baseX, y});
                libList.push_back(libRect);
            }
        }
        std::vector<Position> positions;
        for (size_t i = 0; i < libList.size(); ++i)
            positions.push_back({selectorX, firstY + i * stepY});
        int selected = 0;
        if (!positions.empty()) {
            selector.setPosition(positions.at(selected));
        }
        bool selecting = true;
        while (selecting) {
            EEvent event = _display->pollEvent();
            if (event == QUIT || event == SUPPR || event == ESCAPE) {
                selecting = false;
                running = false;
            }
            if (event == ESCAPE) {
                selecting = false;
            }
            if (event == EEvent::UP && !positions.empty()) {
                selected = (selected - 1 + positions.size()) % positions.size();
                selector.setPosition(positions.at(selected));
            }
            if (event == EEvent::DOWN && !positions.empty()) {
                selected = (selected + 1) % positions.size();
                selector.setPosition(positions.at(selected));
            }
            if (event == EEvent::ENTER && !graphicalLibs.empty()) {
                _display->playSound("./assets/8-bit-click.wav");
                selectedLib = graphicalLibs.at(selected);
                selecting = false;
            }
            if (event == EEvent::TAB) {
                std::string nextGraphic = changeNewDisplay();
                if (!nextGraphic.empty()) {
                    selectedLib = nextGraphic;
                    selecting = false;
                }
            }
            _display->clearWindow();
            for (auto &lib : libList)
                lib->display();
            selector.display();
            _display->render();
        }
        for (auto &handle : handles)
            _loaderGraphic.close(handle);
        return selectedLib;
    }

    std::string Core::runGames(IRect &selector, bool &running)
    {
        std::vector<std::shared_ptr<IRect>> gameList;
        std::vector<void *> handles;
        Size window = _display->getWindowSize();
        size_t buttonW = std::max<size_t>(18, window.w / 3);
        size_t buttonH = std::max<size_t>(3, window.h / 10);
        size_t centerX = window.w / 2;
        size_t centerY = window.h / 2;
        size_t baseX = (centerX > (buttonW / 2)) ? centerX - (buttonW / 2) : 0;
        size_t firstY = (centerY > (buttonH * 2)) ? centerY - (buttonH * 2) : 0;
        size_t stepY = std::max<size_t>(2, buttonH + 1);
        size_t selectorW = std::max<size_t>(2, buttonW / 8);
        size_t selectorX = (baseX > selectorW + 1) ? baseX - (selectorW + 1) : 0;
        std::vector<std::string> gameLibs;
        std::string selectedGraphic;

        for (const auto &lib : _libs) {
            void *handle = _loaderGame.open(lib);
            if (_loaderGame.getType(handle) == EType::GAME) {
                gameLibs.push_back(lib);
                handles.push_back(handle);
                size_t y = firstY + static_cast<size_t>(gameList.size()) * stepY;
                std::shared_ptr<IRect> gameRect = _display->createRect({{buttonW, buttonH}, {baseX, y}});
                gameRect->setTexture({"./assets/button_texture.png", 180, 180, 180, 255});
                gameRect->setText(_loaderGame.getName(handle), {"./assets/font/font.ttf", 255, 255, 255, 255});
                gameRect->setPosition({baseX, y});
                gameList.push_back(gameRect);
            }
        }
        std::vector<Position> positions;
        for (size_t i = 0; i < gameList.size(); ++i)
            positions.push_back({selectorX, firstY + i * stepY});
        int selected = 0;
        if (!positions.empty()) {
            selector.setPosition(positions.at(selected));
        }
        bool selecting = true;
        while (selecting) {
            EEvent event = _display->pollEvent();
            if (event == EEvent::QUIT || event == SUPPR || event == ESCAPE) {
                selecting = false;
                running = false;
            }
            if (event == EEvent::ESCAPE)
                selecting = false;
            if (event == EEvent::UP && !positions.empty()) {
                selected = (selected - 1 + positions.size()) % positions.size();
                selector.setPosition(positions.at(selected));
            }
            if (event == EEvent::DOWN && !positions.empty()) {
                selected = (selected + 1) % positions.size();
                selector.setPosition(positions.at(selected));
            }
            if (event == EEvent::ENTER && !gameLibs.empty()) {
                _display->playSound("./assets/8-bit-click.wav");
                _display->setBackground({"", 50, 50, 50, 255});
                std::string selectedGame = gameLibs.at(selected);
                selectedGraphic = loadGame(selectedGame);
                if (selectedGraphic == CORE_EXIT) {
                    running = false;
                    selecting = false;
                    continue;
                }
                while (!selectedGraphic.empty()) {
                    if (std::find(gameLibs.begin(), gameLibs.end(), selectedGraphic) != gameLibs.end()) {
                        selectedGame = selectedGraphic;
                        _display->setBackground({"", 50, 50, 50, 255});
                        selectedGraphic = loadGame(selectedGame);
                        if (selectedGraphic == CORE_EXIT) {
                            running = false;
                            selecting = false;
                            break;
                        }
                        continue;
                    }
                    loadGraphic(selectedGraphic);
                    _display->setBackground({"", 50, 50, 50, 255});
                    selectedGraphic = loadGame(selectedGame);
                    if (selectedGraphic == CORE_EXIT) {
                        running = false;
                        selecting = false;
                        break;
                    }
                }
                if (!running)
                    continue;
                selecting = false;
            }
            if (event == F1) {
                if (!positions.empty()) {
                    selected = (selected + 1) % positions.size();
                    selector.setPosition(positions.at(selected));
                }
            }
            if (event == EEvent::TAB) {
                std::string nextGraphic = changeNewDisplay();
                if (!nextGraphic.empty()) {
                    selectedGraphic = nextGraphic;
                    selecting = false;
                }
            }
            _display->clearWindow();
            for (auto &game : gameList)
                game->display();
            selector.display();
            _display->render();
        }
        for (auto &handle : handles)
            _loaderGame.close(handle);
        return selectedGraphic;
    }

    bool Core::userName()
    {
        if (!_name.empty())
            return true;
        _name.clear();
        _display->setBackground({"./assets/background.jpg", 0, 0, 0, 255});
        Size window = _display->getWindowSize();
        size_t boxW = std::max<size_t>(24, window.w / 2);
        size_t boxH = std::max<size_t>(3, window.h / 12);
        size_t stepY = std::max<size_t>(2, boxH + 1);
        size_t blockH = (boxH * 3) + (stepY - boxH) * 2;
        size_t baseX = (window.w > boxW) ? (window.w - boxW) / 2 : 0;
        size_t startY = (window.h > blockH) ? (window.h - blockH) / 2 : 0;
        std::shared_ptr<IRect> input = _display->createRect({{boxW, boxH}, {baseX, startY + stepY}});
        input->setTexture({"./assets/button_texture.png", 180, 180, 180, 255});
        input->setText("ENTER YOUR NAME: ", {"./assets/font/font.ttf", 255, 255, 255, 255});
        input->setPosition({baseX, startY});
        std::shared_ptr<IRect> confirm = _display->createRect({{boxW, boxH}, {baseX, startY + (stepY * 2)}});
        confirm->setTexture({"./assets/button_texture.png", 180, 180, 180, 255});
        confirm->setText("PRESS ENTER TO CONFIRM", {"./assets/font/font.ttf", 255, 255, 255, 255});
        confirm->setPosition({baseX, startY + boxH});
        bool selecting = true;
        while (selecting) {
            EEvent event = _display->pollEvent();
            if (event == QUIT || event == SUPPR)
                return false;
            if (event == ESCAPE)
                return false;
            if (event == TAB) {
                std::string nextGraphic = changeNewDisplay();
                if (!nextGraphic.empty()) {
                    input.reset();
                    confirm.reset();
                    loadGraphic(nextGraphic);
                    _display->setBackground({"./assets/background.jpg", 0, 0, 0, 255});
                    window = _display->getWindowSize();
                    boxW = std::max<size_t>(24, window.w / 2);
                    boxH = std::max<size_t>(3, window.h / 12);
                    stepY = std::max<size_t>(2, boxH + 1);
                    blockH = (boxH * 3) + (stepY - boxH) * 2;
                    baseX = (window.w > boxW) ? (window.w - boxW) / 2 : 0;
                    startY = (window.h > blockH) ? (window.h - blockH) / 2 : 0;
                    input = _display->createRect({{boxW, boxH}, {baseX, startY + stepY}});
                    input->setTexture({"./assets/button_texture.png", 180, 180, 180, 255});
                    input->setPosition({baseX, startY});
                    confirm = _display->createRect({{boxW, boxH}, {baseX, startY + (stepY * 2)}});
                    confirm->setTexture({"./assets/button_texture.png", 180, 180, 180, 255});
                    confirm->setText("PRESS ENTER TO CONFIRM", {"./assets/font/font.ttf", 255, 255, 255, 255});
                    confirm->setPosition({baseX, startY + boxH});
                }
                continue;
            }
            if (event == BACKSPACE && !_name.empty())
                _name.pop_back();
            char typedCharacter = eventToChar(event);
            if (typedCharacter != '\0' && _name.size() < 12)
                _name.push_back(typedCharacter);
            if (event == ENTER) {
                if (_name.empty())
                    _name = "PLAYER";
                selecting = false;
            }
            input->setText("NAME: " + _name, {"./assets/font/font.ttf", 255, 255, 255, 255});
            _display->clearWindow();
            input->display();
            confirm->display();
            _display->render();
        }
        return true;
    }

    void Core::displayScore()
    {
        std::vector<ScoreEntry> entries;
        std::ifstream file("./scores.txt");
        if (file.is_open()) {
            std::string line;
            while (std::getline(file, line)) {
                std::stringstream iss(line);
                ScoreEntry entry;
                if (iss >> entry.name >> entry.game >> entry.score)
                    entries.push_back(entry);
            }
            file.close();
        }
        bool showingScores = true;

        while (showingScores) {
            Size window = _display->getWindowSize();
            size_t colW = std::max<size_t>(14, window.w / 5);
            size_t rowH = std::max<size_t>(3,  window.h / 14);
            size_t stepY = std::max<size_t>(2,  rowH + 1);
            size_t tableW = colW * 3 + 2;
            size_t centerX = window.w / 2;
            size_t baseX = (centerX > tableW / 2) ? centerX - tableW / 2 : 0;
            size_t col1X = baseX;
            size_t col2X = baseX + colW + 1;
            size_t col3X = baseX + (colW + 1) * 2;
            size_t maxRows = 8;
            size_t totalRows = std::min(entries.size(), maxRows);
            size_t tableH = (totalRows + 1) * stepY;
            size_t startY = (window.h > tableH) ? (window.h - tableH) / 2 : 0;

            std::shared_ptr<IRect> name = _display->createRect({{colW, rowH}, {col1X, startY}});
            std::shared_ptr<IRect> game = _display->createRect({{colW, rowH}, {col2X, startY}});
            std::shared_ptr<IRect> score = _display->createRect({{colW, rowH}, {col3X, startY}});
            name->setTexture({"./assets/button_texture.png", 80, 80, 160, 255});
            game->setTexture({"./assets/button_texture.png", 80, 80, 160, 255});
            score->setTexture({"./assets/button_texture.png", 80, 80, 160, 255});
            name->setText("PLAYER", {"./assets/font/font.ttf", 255, 220, 50, 255});
            game->setText("GAME", {"./assets/font/font.ttf", 255, 220, 50, 255});
            score->setText("SCORE", {"./assets/font/font.ttf", 255, 220, 50, 255});

            std::vector<std::shared_ptr<IRect>> rows;
            for (size_t i = 0; i < totalRows; ++i) {
                size_t y = startY + (i + 1) * stepY;
                auto nameRect = _display->createRect({{colW, rowH}, {col1X, y}});
                auto gameRect = _display->createRect({{colW, rowH}, {col2X, y}});
                auto scoreRect = _display->createRect({{colW, rowH}, {col3X, y}});
                nameRect->setTexture({"./assets/button_texture.png", 180, 180, 180, 255});
                gameRect->setTexture({"./assets/button_texture.png", 180, 180, 180, 255});
                scoreRect->setTexture({"./assets/button_texture.png", 180, 180, 180, 255});
                nameRect->setText(entries[i].name, {"./assets/font/font.ttf", 255, 255, 255, 255});
                gameRect->setText(entries[i].game, {"./assets/font/font.ttf", 255, 255, 255, 255});
                scoreRect->setText(std::to_string(entries[i].score), {"./assets/font/font.ttf", 255, 255, 255, 255});
                rows.push_back(nameRect);
                rows.push_back(gameRect);
                rows.push_back(scoreRect);
            }

            bool running = true;
            bool switchDisplay = false;
            std::string nextGraphic;

            while (running) {
                EEvent event = _display->pollEvent();
                if (event == QUIT || event == SUPPR) {
                    running = false;
                    showingScores = false;
                }
                if (event == ESCAPE) {
                    running = false;
                    showingScores = false;
                }
                if (event == TAB) {
                    nextGraphic = changeNewDisplay();
                    if (!nextGraphic.empty()) {
                        switchDisplay = true;
                        running = false;
                    }
                }
                _display->clearWindow();
                name->display();
                game->display();
                score->display();
                for (auto &rect : rows)
                    rect->display();
                _display->render();
            }

            rows.clear();
            name.reset();
            game.reset();
            score.reset();

            if (switchDisplay) {
                loadGraphic(nextGraphic);
                _display->setBackground({"./assets/background.jpg", 0, 0, 0, 255});
            }
        }
    }

    void Core::run(const std::string &lib)
    {
        _libs = initLibList(lib);
        loadGraphic(lib);
        if (!userName()) {
            _display->stop();
            _display.reset();
            _loaderGraphic.close(_handleGraphic);
            _handleGraphic = nullptr;
            return;
        }
        Size window = _display->getWindowSize();
        size_t buttonW = std::max<size_t>(18, window.w / 4);
        size_t buttonH = std::max<size_t>(3, window.h / 12);
        size_t centerX = window.w / 2;
        size_t baseX = (centerX > (buttonW / 2)) ? centerX - (buttonW / 2) : 0;
        size_t stepY = std::max<size_t>(2, buttonH + 1);
        size_t menuHeight = (buttonH * 5) + ((stepY - buttonH) * 4);
        size_t firstY = (window.h > menuHeight) ? (window.h - menuHeight) / 2 : 0;

        std::shared_ptr<IRect> playButton = _display->createRect({{buttonW, buttonH}, {baseX, firstY}});
        playButton->setTexture({"./assets/button_texture.png", 180, 180, 180, 255});
        playButton->setText("PLAY", {"./assets/font/font.ttf", 255, 255, 255, 255});
        playButton->setPosition({baseX, firstY});
        std::shared_ptr<IRect> changeLibButton = _display->createRect({{buttonW, buttonH}, {baseX, firstY + stepY}});
        changeLibButton->setTexture({"./assets/button_texture.png", 180, 180, 180, 255});
        changeLibButton->setText("LIB", {"./assets/font/font.ttf", 255, 255, 255, 255});
        changeLibButton->setPosition({baseX, firstY + stepY});
        std::shared_ptr<IRect> exit = _display->createRect({{buttonW, buttonH}, {baseX, firstY + (stepY * 2)}});
        exit->setTexture({"./assets/button_texture.png", 180, 180, 180, 255});
        exit->setText("EXIT", {"./assets/font/font.ttf", 255, 255, 255, 255});
        exit->setPosition({baseX, firstY + (stepY * 2)});
        std::shared_ptr<IRect> scores = _display->createRect({{buttonW, buttonH}, {baseX, firstY + (stepY * 3)}});
        scores->setTexture({"./assets/button_texture.png", 180, 180, 180, 255});
        scores->setText("SCORE", {"./assets/font/font.ttf", 255, 255, 255, 255});
        scores->setPosition({baseX, firstY + (stepY * 3)});
        std::shared_ptr<IRect> nameField = _display->createRect({{buttonW, buttonH}, {baseX, firstY + (stepY * 3)}});
        nameField->setTexture({"./assets/button_texture.png", 180, 180, 180, 255});
        nameField->setText("NAME: " + _name, {"./assets/font/font.ttf", 255, 255, 255, 255});
        nameField->setPosition({baseX, firstY + (stepY * 4)});
        _display->setBackground({"./assets/background.jpg", 0, 0, 0, 255});
        std::shared_ptr<IRect> selector = _display->createRect({{std::max<size_t>(2, buttonW / 8), buttonH}, {0, 0}});
        selector->setTexture({"./assets/selector.png", 255, 255, 255, 255});
        size_t selectorX = (baseX > std::max<size_t>(2, buttonW / 8) + 1) ? baseX - (std::max<size_t>(2, buttonW / 8) + 1) : 0;
        std::vector<Position> positions = {{selectorX, firstY}, {selectorX, firstY + stepY}, {selectorX, firstY + (stepY * 2)}, {selectorX, firstY + (stepY * 3)}, {selectorX, firstY + (stepY * 4)}};
        int selected = 0;
        selector->setPosition(positions.at(selected));
        bool running = true;
        while (running) {
            EEvent event = _display->pollEvent();
            if (event == QUIT || event == SUPPR || event == ESCAPE)
                running = false;
            if (event == UP) {
                selected = (selected - 1 + positions.size()) % positions.size();
                selector->setPosition(positions.at(selected));
            }
            if (event == DOWN) {
                selected = (selected + 1) % positions.size();
                selector->setPosition(positions.at(selected));
            }
            if (event == ENTER || event == TAB) {
                std::string selectedGraphic;
                if (event == ENTER) {
                    _display->playSound("./assets/8-bit-click.wav");
                    if (selected == 0)
                        selectedGraphic = runGames(*selector, running);
                    else if (selected == 1)
                        selectedGraphic = runlib(*selector, running);
                    else if (selected == 3)
                        displayScore();
                    else if (selected == 4) {
                        std::string previousName = _name;
                        _name.clear();
                        if (!userName())
                            _name = "PLAYER";
                    }
                    else
                        running = false;
                }
                if (event == TAB)
                    selectedGraphic = changeNewDisplay();
                if (!running)
                    break;
                if (!selectedGraphic.empty()) {
                    playButton.reset();
                    changeLibButton.reset();
                    exit.reset();
                    scores.reset();
                    nameField.reset();
                    selector.reset();
                    loadGraphic(selectedGraphic);
                }
                window = _display->getWindowSize();
                buttonW = std::max<size_t>(18, window.w / 4);
                buttonH = std::max<size_t>(3, window.h / 12);
                centerX = window.w / 2;
                baseX = (centerX > (buttonW / 2)) ? centerX - (buttonW / 2) : 0;
                stepY = std::max<size_t>(2, buttonH + 1);
                menuHeight = (buttonH * 5) + ((stepY - buttonH) * 4);
                firstY = (window.h > menuHeight) ? (window.h - menuHeight) / 2 : 0;
                selectorX = (baseX > std::max<size_t>(2, buttonW / 8) + 1) ? baseX - (std::max<size_t>(2, buttonW / 8) + 1) : 0;
                positions = {{selectorX, firstY}, {selectorX, firstY + stepY}, {selectorX, firstY + (stepY * 2)}, {selectorX, firstY + (stepY * 3)}, {selectorX, firstY + (stepY * 4)}};
                playButton = _display->createRect({{buttonW, buttonH}, {baseX, firstY}});
                playButton->setTexture({"./assets/button_texture.png", 180, 180, 180, 255});
                playButton->setText("PLAY", {"./assets/font/font.ttf", 255, 255, 255, 255});
                playButton->setPosition({baseX, firstY});
                changeLibButton = _display->createRect({{buttonW, buttonH}, {baseX, firstY + stepY}});
                changeLibButton->setTexture({"./assets/button_texture.png", 180, 180, 180, 255});
                changeLibButton->setText("LIB", {"./assets/font/font.ttf", 255, 255, 255, 255});
                changeLibButton->setPosition({baseX, firstY + stepY});
                exit = _display->createRect({{buttonW, buttonH}, {baseX, firstY + (stepY * 2)}});
                exit->setTexture({"./assets/button_texture.png", 180, 180, 180, 255});
                exit->setText("EXIT", {"./assets/font/font.ttf", 255, 255, 255, 255});
                exit->setPosition({baseX, firstY + (stepY * 2)});
                scores = _display->createRect({{buttonW, buttonH}, {baseX, firstY + (stepY * 3)}});
                scores->setTexture({"./assets/button_texture.png", 180, 180, 180, 255});
                scores->setText("SCORE", {"./assets/font/font.ttf", 255, 255, 255, 255});
                scores->setPosition({baseX, firstY + (stepY * 3)});
                nameField = _display->createRect({{buttonW, buttonH}, {baseX, firstY + (stepY * 4)}});
                nameField->setTexture({"./assets/button_texture.png", 180, 180, 180, 255});
                nameField->setText("NAME: " + _name, {"./assets/font/font.ttf", 255, 255, 255, 255});
                nameField->setPosition({baseX, firstY + (stepY * 4)});
                selector = _display->createRect({{std::max<size_t>(2, buttonW / 8), buttonH}, {0, 0}});
                selector->setTexture({"./assets/selector.png", 255, 255, 255, 255});
                selector->setPosition(positions.at(selected));
                _display->setBackground({"./assets/background.jpg", 0, 0, 0, 255});
            }
            nameField->setText("NAME: " + _name, {"./assets/font/font.ttf", 255, 255, 255, 255});
            _display->clearWindow();
            playButton->display();
            changeLibButton->display();
            exit->display();
            scores->display();
            nameField->display();
            selector->display();
            _display->render();
        }
        playButton.reset();
        changeLibButton.reset();
        exit.reset();
        scores.reset();
        nameField.reset();
        selector.reset();
        _display->stop();
        _display.reset();
        _loaderGraphic.close(_handleGraphic);
        _handleGraphic = nullptr;
    }

    void Core::loadGraphic(const std::string &path)
    {
        if (_display != nullptr) {
            _display->stop();
            _display.reset();
            _loaderGraphic.close(_handleGraphic);
            _handleGraphic = nullptr;
        }
        _handleGraphic = _loaderGraphic.open(path);
        _type = _loaderGraphic.getType(_handleGraphic);
        if (_type == EType::GRAPHICAL) {
            _display = std::shared_ptr<IDisplayModule>(_loaderGraphic.getInstance(_handleGraphic));
            _graphicName = _loaderGraphic.getName(_handleGraphic);
            _currentGraphicPath = path;
            _display->init(_graphicName, {1920, 1080});
        } else
            throw Exception("Error: '" + path + "' not a graphical library");
    }
}