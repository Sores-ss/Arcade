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
        playButton->setPosition(1920 / 2 - 100, 1080 / 2 - 100);
        bool running = true;
        while (running) {
            EEvent event = _display->pollEvent();
            if (event == EEvent::QUIT || event == EEvent::ESCAPE)
                running = false;
            _display->clear();
            _display->displayRect(playButton);
            _display->render();
        }
        _display->stop();
        _loader.close(_handle);
    }

    void Core::loadLib(const std::string &path)
    {
        _handle = _loader.open(path);
        _type = _loader.getType(_handle);
        if (_type == EType::GRAPHICAL) {
            _display = _loader.getInstance(_handle);
            _name = _loader.getName(_handle);
            if (_name == "SDL2")
                _display->init("SDL2", 1920, 1080);
        } else
            throw Exception("Error: '" + path + "' not a graphical library");
    }
}