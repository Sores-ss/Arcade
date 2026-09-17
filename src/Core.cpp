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

    void Core::run(const std::string &lib)
    {
        _libs = initLibList(lib);
        loadLib(lib);
        IRect *playButton = _display->createRect(200, 100, 300, 250);
        _display->setTexture(playButton, {"./assets/button_texture.png", 180, 180, 180, 255});
        _display->setText(playButton, "PLAY", {"./assets/Pixellettersfull-BnJ5.ttf", 255, 255, 255, 255});
        playButton->setPosition(1920 / 2 - 100, 1080 / 2 - 200 );
        IRect *changeLibButton = _display->createRect(200, 100, 300, 250);
        _display->setTexture(changeLibButton, {"./assets/button_texture.png", 180, 180, 180, 255});
        _display->setText(changeLibButton, "LIB", {"./assets/Pixellettersfull-BnJ5.ttf", 255, 255, 255, 255});
        changeLibButton->setPosition(1920 / 2 - 100, 1080 / 2 - 50);
        IRect *exit = _display->createRect(200, 100, 300, 250);
        _display->setTexture(exit, {"./assets/button_texture.png", 180, 180, 180, 255});
        _display->setText(exit, "EXIT", {"./assets/Pixellettersfull-BnJ5.ttf", 255, 255, 255, 255});
        exit->setPosition(1920 / 2 - 100, 1080 / 2 + 100);
        bool running = true;
        while (running) {
            EEvent event = _display->pollEvent();
            if (event == EEvent::QUIT || event == EEvent::ESCAPE)
                running = false;
            _display->clear();
            _display->displayRect(playButton);
            _display->displayRect(changeLibButton);
            _display->displayRect(exit);
            _display->render();
        }
        _display->stop();
        _loaderGraphic.close(_handleGraphic);
    }

    void Core::loadLib(const std::string &path)
    {
        _handleGraphic = _loaderGraphic.open(path);
        _type = _loaderGraphic.getType(_handleGraphic);
        if (_type == EType::GRAPHICAL) {
            _display = _loaderGraphic.getInstance(_handleGraphic);
            _name = _loaderGraphic.getName(_handleGraphic);
            if (_name == "SDL2")
                _display->init("SDL2", 1920, 1080);
        } else
            throw Exception("Error: '" + path + "' not a graphical library");
    }
}