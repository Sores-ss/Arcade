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

    void Core::loadGame(const std::string &path)
    {
        _handleGame = _loaderGame.open(path);
        EType type = _loaderGame.getType(_handleGame);
        if (type == EType::GAME) {
            _game = _loaderGame.getInstance(_handleGame);
            _game->run(_display);
        } else
            throw Exception("Error: invalid game library : " + path);
    }

    std::string Core::runlib(IRect &selector, bool &running)
    {
        std::vector<IRect *> libList;
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
                IRect *libRect = _display->createRect({buttonW, buttonH, baseX, y});
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
            _display->clearWindow();
            for (auto &lib : libList)
                lib->display();
            selector.display();
            _display->render();
        }
        for (auto &lib : libList)
            delete lib;
        for (auto &handle : handles)
            _loaderGraphic.close(handle);
        return selectedLib;
    }

    void Core::runGames(IRect &selector, bool &running)
    {
        std::vector<IRect *> gameList;
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

        for (const auto &lib : _libs) {
            void *handle = _loaderGame.open(lib);
            if (_loaderGame.getType(handle) == EType::GAME) {
                gameLibs.push_back(lib);
                handles.push_back(handle);
                size_t y = firstY + static_cast<size_t>(gameList.size()) * stepY;
                IRect *gameRect = _display->createRect({buttonW, buttonH, baseX, y});
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
                loadGame(gameLibs.at(selected));
                selecting = false;
            }
            _display->clearWindow();
            for (auto &game : gameList)
                game->display();
            selector.display();
            _display->render();
        }
        for (auto &game : gameList)
            delete game;
        for (auto &handle : handles)
            _loaderGame.close(handle);
        if (_game != nullptr)
            _loaderGame.close(_handleGame);
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

        IRect *playButton = _display->createRect({buttonW, buttonH, baseX, firstY});
        playButton->setTexture({"./assets/button_texture.png", 180, 180, 180, 255});
        playButton->setText("PLAY", {"./assets/Pixellettersfull-BnJ5.ttf", 255, 255, 255, 255});
        playButton->setPosition({baseX, firstY});
        IRect *changeLibButton = _display->createRect({buttonW, buttonH, baseX, firstY + stepY});
        changeLibButton->setTexture({"./assets/button_texture.png", 180, 180, 180, 255});
        changeLibButton->setText("LIB", {"./assets/Pixellettersfull-BnJ5.ttf", 255, 255, 255, 255});
        changeLibButton->setPosition({baseX, firstY + stepY});
        IRect *exit = _display->createRect({buttonW, buttonH, baseX, firstY + (stepY * 2)});
        exit->setTexture({"./assets/button_texture.png", 180, 180, 180, 255});
        exit->setText("EXIT", {"./assets/Pixellettersfull-BnJ5.ttf", 255, 255, 255, 255});
        exit->setPosition({baseX, firstY + (stepY * 2)});
        _display->setBackground({"./assets/background.jpg", 0, 0, 0, 255});
        IRect *selector = _display->createRect({std::max<size_t>(2, buttonW / 8), buttonH, 0, 0});
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
            if (event == EEvent::ENTER) {
                _display->playSound("./assets/8-bit-click.wav");
                std::string selectedGraphic;
                if (selected == 0)
                    runGames(*selector, running);
                else if (selected == 1)
                    selectedGraphic = runlib(*selector, running);
                else
                    running = false;
                
                if (!running)
                    break;
                delete playButton;
                delete changeLibButton;
                delete exit;
                delete selector;
                if (!selectedGraphic.empty())
                    loadGraphic(selectedGraphic);
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
        delete playButton;
        delete changeLibButton;
        delete exit;
        delete selector;
        _display->stop();
        _loaderGraphic.close(_handleGraphic);
    }

    void Core::loadGraphic(const std::string &path)
    {
        if (_display != nullptr) {
            _display->stop();
            _loaderGraphic.close(_handleGraphic);
        }
        _handleGraphic = _loaderGraphic.open(path);
        _type = _loaderGraphic.getType(_handleGraphic);
        if (_type == EType::GRAPHICAL) {
            _display = _loaderGraphic.getInstance(_handleGraphic);
            _name = _loaderGraphic.getName(_handleGraphic);
            _display->init(_name, {1920, 1080});
        } else
            throw Exception("Error: '" + path + "' not a graphical library");
    }
}