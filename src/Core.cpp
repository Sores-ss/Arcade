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

namespace arcade {
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
        _game->run(_display);
        bool wantsDisplayChange = _game->changeDisplay();
        if (wantsDisplayChange)
            return changeDisplay();
        _game.reset();
        _loaderGame.close(_handleGame);
        _handleGame = nullptr;
        _currentGamePath.clear();
        return "";
    }

    std::string Core::changeDisplay()
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
                std::shared_ptr<IRect> libRect = _display->createRect({buttonW, buttonH, baseX, y});
                libRect->setTexture({"./assets/button_texture.png", 180, 180, 180, 255});
                libRect->setText(_loaderGraphic.getName(handle), {"./assets/Pixellettersfull-BnJ5.ttf", 255, 255, 255, 255});
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
            if (event == EEvent::QUIT) {
                selecting = false;
                running = false;
            }
            if (event == EEvent::ESCAPE) {
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
                std::string nextGraphic = changeDisplay();
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
                std::shared_ptr<IRect> gameRect = _display->createRect({buttonW, buttonH, baseX, y});
                gameRect->setTexture({"./assets/button_texture.png", 180, 180, 180, 255});
                gameRect->setText(_loaderGame.getName(handle), {"./assets/Pixellettersfull-BnJ5.ttf", 255, 255, 255, 255});
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
            if (event == EEvent::QUIT) {
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
                while (!selectedGraphic.empty()) {
                    loadGraphic(selectedGraphic);
                    _display->setBackground({"", 50, 50, 50, 255});
                    selectedGraphic = loadGame(selectedGame);
                }
                selecting = false;
            }
            if (event == EEvent::TAB) {
                std::string nextGraphic = changeDisplay();
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

    void Core::run(const std::string &lib)
    {
        _libs = initLibList(lib);
        loadGraphic(lib);
        Size window = _display->getWindowSize();
        size_t buttonW = std::max<size_t>(18, window.w / 4);
        size_t buttonH = std::max<size_t>(3, window.h / 12);
        size_t centerX = window.w / 2;
        size_t centerY = window.h / 2;
        size_t baseX = (centerX > (buttonW / 2)) ? centerX - (buttonW / 2) : 0;
        size_t firstY = (centerY > (buttonH * 2)) ? centerY - (buttonH * 2) : 0;
        size_t stepY = std::max<size_t>(2, buttonH + 1);

        std::shared_ptr<IRect> playButton = _display->createRect({buttonW, buttonH, baseX, firstY});
        playButton->setTexture({"./assets/button_texture.png", 180, 180, 180, 255});
        playButton->setText("PLAY", {"./assets/Pixellettersfull-BnJ5.ttf", 255, 255, 255, 255});
        playButton->setPosition({baseX, firstY});
        std::shared_ptr<IRect> changeLibButton = _display->createRect({buttonW, buttonH, baseX, firstY + stepY});
        changeLibButton->setTexture({"./assets/button_texture.png", 180, 180, 180, 255});
        changeLibButton->setText("LIB", {"./assets/Pixellettersfull-BnJ5.ttf", 255, 255, 255, 255});
        changeLibButton->setPosition({baseX, firstY + stepY});
        std::shared_ptr<IRect> exit = _display->createRect({buttonW, buttonH, baseX, firstY + (stepY * 2)});
        exit->setTexture({"./assets/button_texture.png", 180, 180, 180, 255});
        exit->setText("EXIT", {"./assets/Pixellettersfull-BnJ5.ttf", 255, 255, 255, 255});
        exit->setPosition({baseX, firstY + (stepY * 2)});
        _display->setBackground({"./assets/background.jpg", 0, 0, 0, 255});
        std::shared_ptr<IRect> selector = _display->createRect({std::max<size_t>(2, buttonW / 8), buttonH, 0, 0});
        selector->setTexture({"./assets/selector.png", 255, 255, 255, 255});
        size_t selectorX = (baseX > std::max<size_t>(2, buttonW / 8) + 1) ? baseX - (std::max<size_t>(2, buttonW / 8) + 1) : 0;
        std::vector<Position> positions = {{selectorX, firstY}, {selectorX, firstY + stepY}, {selectorX, firstY + (stepY * 2)}};
        int selected = 0;
        selector->setPosition(positions.at(selected));
        bool running = true;
        while (running) {
            EEvent event = _display->pollEvent();
            if (event == EEvent::QUIT || event == EEvent::ESCAPE)
                running = false;
            if (event == EEvent::UP) {
                selected = (selected - 1 + positions.size()) % positions.size();
                selector->setPosition(positions.at(selected));
            }
            if (event == EEvent::DOWN) {
                selected = (selected + 1) % positions.size();
                selector->setPosition(positions.at(selected));
            }
            if (event == EEvent::ENTER || event == EEvent::TAB) {
                std::string selectedGraphic;
                if (event == EEvent::ENTER) {
                    _display->playSound("./assets/8-bit-click.wav");
                    if (selected == 0)
                        selectedGraphic = runGames(*selector, running);
                    else if (selected == 1)
                        selectedGraphic = runlib(*selector, running);
                    else
                        running = false;
                }
                if (event == EEvent::TAB)
                    selectedGraphic = changeDisplay();
                if (!running)
                    break;
                if (!selectedGraphic.empty()) {
                    playButton.reset();
                    changeLibButton.reset();
                    exit.reset();
                    selector.reset();
                    loadGraphic(selectedGraphic);
                }
                window = _display->getWindowSize();
                buttonW = std::max<size_t>(18, window.w / 4);
                buttonH = std::max<size_t>(3, window.h / 12);
                centerX = window.w / 2;
                centerY = window.h / 2;
                baseX = (centerX > (buttonW / 2)) ? centerX - (buttonW / 2) : 0;
                firstY = (centerY > (buttonH * 2)) ? centerY - (buttonH * 2) : 0;
                stepY = std::max<size_t>(2, buttonH + 1);
                selectorX = (baseX > std::max<size_t>(2, buttonW / 8) + 1) ? baseX - (std::max<size_t>(2, buttonW / 8) + 1) : 0;
                positions = {{selectorX, firstY}, {selectorX, firstY + stepY}, {selectorX, firstY + (stepY * 2)}};
                playButton = _display->createRect({buttonW, buttonH, baseX, firstY});
                playButton->setTexture({"./assets/button_texture.png", 180, 180, 180, 255});
                playButton->setText("PLAY", {"./assets/Pixellettersfull-BnJ5.ttf", 255, 255, 255, 255});
                playButton->setPosition({baseX, firstY});
                changeLibButton = _display->createRect({buttonW, buttonH, baseX, firstY + stepY});
                changeLibButton->setTexture({"./assets/button_texture.png", 180, 180, 180, 255});
                changeLibButton->setText("LIB", {"./assets/Pixellettersfull-BnJ5.ttf", 255, 255, 255, 255});
                changeLibButton->setPosition({baseX, firstY + stepY});
                exit = _display->createRect({buttonW, buttonH, baseX, firstY + (stepY * 2)});
                exit->setTexture({"./assets/button_texture.png", 180, 180, 180, 255});
                exit->setText("EXIT", {"./assets/Pixellettersfull-BnJ5.ttf", 255, 255, 255, 255});
                exit->setPosition({baseX, firstY + (stepY * 2)});
                selector = _display->createRect({std::max<size_t>(2, buttonW / 8), buttonH, 0, 0});
                selector->setTexture({"./assets/selector.png", 255, 255, 255, 255});
                selector->setPosition(positions.at(selected));
                _display->setBackground({"./assets/background.jpg", 0, 0, 0, 255});
            }
            _display->clearWindow();
            playButton->display();
            changeLibButton->display();
            exit->display();
            selector->display();
            _display->render();
        }
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
            _name = _loaderGraphic.getName(_handleGraphic);
            _currentGraphicPath = path;
            _display->init(_name, {1920, 1080});
        } else
            throw Exception("Error: '" + path + "' not a graphical library");
    }
}