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
        try {
            _handleGame = _loaderGame.open(path);
            EType type = _loaderGame.getType(_handleGame);
            if (type == EType::GAME) {
                _game = _loaderGame.getInstance(_handleGame);
                _game->run(_display);
            }
        } catch (const std::exception &e) {
            throw Exception(std::string("Error loading game: ") + e.what());
        }
    }

    void Core::runlib(IRect &selector, bool &running)
    {
        std::vector<IRect *> libList;
        std::vector<void *> handles;
        Size window = _display->getWindowSize();
        std::vector<std::string> graphicalLibs;

        for (const auto &lib : _libs) {
            try {
                void *handle = _loaderGraphic.open(lib);
                if (_loaderGraphic.getType(handle) == EType::GRAPHICAL) {
                    graphicalLibs.push_back(lib);
                    handles.push_back(handle);
                    IRect *libRect = _display->createRect({200, 100, 300, 250});
                    _display->setTexture(*libRect, {"./assets/button_texture.png", 180, 180, 180, 255});
                    _display->setText(*libRect, _loaderGraphic.getName(handle), {"./assets/Pixellettersfull-BnJ5.ttf", 255, 255, 255, 255});
                    libRect->setPosition({window.w / 2 - 100, window.h / 2 - 175 + static_cast<int>(libList.size()) * 150});
                    libList.push_back(libRect);
                }
            } catch (const std::exception &e) {
                std::cerr << "Error loading lib " << lib << ": " << e.what() << std::endl;
            }
        }
        std::vector<Position> positions;
        for (size_t i = 0; i < libList.size(); ++i)
            positions.push_back({window.w / 2 - 150, window.h / 2 - 150 + static_cast<int>(i) * 150});
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
            if (event == EEvent::UP) {
                selected = (selected - 1 + positions.size()) % positions.size();
                selector.setPosition(positions.at(selected));
            }
            if (event == EEvent::DOWN) {
                selected = (selected + 1) % positions.size();
                selector.setPosition(positions.at(selected));
            }
            if (event == EEvent::ENTER) {
                try {
                    loadGraphic(graphicalLibs.at(selected));
                } catch (const std::exception &e) {
                    std::cerr << "Error loading library: " << e.what() << std::endl;
                }
                selecting = false;
            }
            _display->clearWindow();
            for (auto &lib : libList)
                _display->displayRect(*lib);
            _display->displayRect(selector);
            _display->render();
        }
        for (auto &lib : libList)
            delete lib;
        for (auto &handle : handles)
            _loaderGraphic.close(handle);
    }

    void Core::runGames(IRect &selector, bool &running)
    {
        std::vector<IRect *> gameList;
        std::vector<void *> handles;
        Size window = _display->getWindowSize();
        std::vector<std::string> gameLibs;

        for (const auto &lib : _libs) {
            try {
                void *handle = _loaderGame.open(lib);
                if (_loaderGame.getType(handle) == EType::GAME) {
                    gameLibs.push_back(lib);
                    handles.push_back(handle);
                    IRect *gameRect = _display->createRect({200, 100, 300, 250});
                    _display->setTexture(*gameRect, {"./assets/button_texture.png", 180, 180, 180, 255});
                    _display->setText(*gameRect, _loaderGame.getName(handle), {"./assets/Pixellettersfull-BnJ5.ttf", 255, 255, 255, 255});
                    gameRect->setPosition({window.w / 2 - 100, window.h / 2 - 175 + static_cast<int>(gameList.size()) * 150});
                    gameList.push_back(gameRect);
                }
            } catch (const std::exception &e) {
                std::cerr << "Error loading game " << lib << ": " << e.what() << std::endl;
            }
        }
        std::vector<Position> positions;
        for (size_t i = 0; i < gameList.size(); ++i)
            positions.push_back({window.w / 2 - 150, window.h / 2 - 150 + static_cast<int>(i) * 150});
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
            if (event == EEvent::UP) {
                selected = (selected - 1 + positions.size()) % positions.size();
                selector.setPosition(positions.at(selected));
            }
            if (event == EEvent::DOWN) {
                selected = (selected + 1) % positions.size();
                selector.setPosition(positions.at(selected));
            }
            if (event == EEvent::ENTER) {
                loadGame(gameLibs.at(selected));
                selecting = false;
            }
            _display->clearWindow();
            for (auto &game : gameList)
                _display->displayRect(*game);
            _display->displayRect(selector);
            _display->render();
        }
        for (auto &game : gameList)
            delete game;
        for (auto &handle : handles)
            _loaderGame.close(handle);
        if (_game != nullptr) {
            _game->stop();
            _loaderGame.close(_handleGame);
        }
    }

    void Core::run(const std::string &lib)
    {
        _libs = initLibList(lib);
        loadGraphic(lib);
        IRect *playButton = _display->createRect({200, 100, 300, 250});
        _display->setTexture(*playButton, {"./assets/button_texture.png", 180, 180, 180, 255});
        _display->setText(*playButton, "PLAY", {"./assets/Pixellettersfull-BnJ5.ttf", 255, 255, 255, 255});
        playButton->setPosition({1920 / 2 - 100, 1080 / 2 - 200});
        IRect *changeLibButton = _display->createRect({200, 100, 300, 250});
        _display->setTexture(*changeLibButton, {"./assets/button_texture.png", 180, 180, 180, 255});
        _display->setText(*changeLibButton, "LIB", {"./assets/Pixellettersfull-BnJ5.ttf", 255, 255, 255, 255});
        changeLibButton->setPosition({1920 / 2 - 100, 1080 / 2 - 50});
        IRect *exit = _display->createRect({200, 100, 300, 250});
        _display->setTexture(*exit, {"./assets/button_texture.png", 180, 180, 180, 255});
        _display->setText(*exit, "EXIT", {"./assets/Pixellettersfull-BnJ5.ttf", 255, 255, 255, 255});
        exit->setPosition({1920 / 2 - 100, 1080 / 2 + 100});
        _display->setBackground({"./assets/background.jpg", 0, 0, 0, 255});
        IRect *selector = _display->createRect({50, 50, 0, 0});
        _display->setTexture(*selector, {"./assets/selector.png", 255, 255, 255, 255});
        std::vector<Position> positions = {{1920 / 2 - 150, 1080 / 2 - 175}, {1920 / 2 - 150, 1080 / 2 - 25}, {1920 / 2 - 150, 1080 / 2 + 125}};
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
                if (selected == 0) {
                    runGames(*selector, running);
                } else if (selected == 1) {
                    runlib(*selector, running);
                } else
                    running = false;
                
                if (!running)
                    break;
                delete playButton;
                delete changeLibButton;
                delete exit;
                delete selector;
                playButton = _display->createRect({200, 100, 300, 250});
                _display->setTexture(*playButton, {"./assets/button_texture.png", 180, 180, 180, 255});
                _display->setText(*playButton, "PLAY", {"./assets/Pixellettersfull-BnJ5.ttf", 255, 255, 255, 255});
                playButton->setPosition({1920 / 2 - 100, 1080 / 2 - 200});
                changeLibButton = _display->createRect({200, 100, 300, 250});
                _display->setTexture(*changeLibButton, {"./assets/button_texture.png", 180, 180, 180, 255});
                _display->setText(*changeLibButton, "LIB", {"./assets/Pixellettersfull-BnJ5.ttf", 255, 255, 255, 255});
                changeLibButton->setPosition({1920 / 2 - 100, 1080 / 2 - 50});
                exit = _display->createRect({200, 100, 300, 250});
                _display->setTexture(*exit, {"./assets/button_texture.png", 180, 180, 180, 255});
                _display->setText(*exit, "EXIT", {"./assets/Pixellettersfull-BnJ5.ttf", 255, 255, 255, 255});
                exit->setPosition({1920 / 2 - 100, 1080 / 2 + 100});
                selector = _display->createRect({50, 50, 0, 0});
                _display->setTexture(*selector, {"./assets/selector.png", 255, 255, 255, 255});
                selector->setPosition(positions.at(selected));
                _display->setBackground({"./assets/background.jpg", 0, 0, 0, 255});
            }
            _display->clearWindow();
            _display->displayRect(*playButton);
            _display->displayRect(*changeLibButton);
            _display->displayRect(*exit);
            _display->displayRect(*selector);
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
        try {
            if (_display != nullptr) {
                _display->stop();
                _loaderGraphic.close(_handleGraphic);
            }
            _handleGraphic = _loaderGraphic.open(path);
            _type = _loaderGraphic.getType(_handleGraphic);
            if (_type == EType::GRAPHICAL) {
                _display = _loaderGraphic.getInstance(_handleGraphic);
                _name = _loaderGraphic.getName(_handleGraphic);
                if (_name == "SDL2")
                    _display->init("SDL2", {1920, 1080});
            } else
                throw Exception("Error: '" + path + "' not a graphical library");
        } catch (const std::exception &e) {
            throw Exception(std::string("Error loading graphic library: ") + e.what());
        }
    }
}