/*
** EPITECH PROJECT, 2025
** G-OOP-400-LIL-4-1-arcade-9
** File description:
** core.hpp
*/

#pragma once

#include <vector>
#include <iostream>
#include "IDisplayModule.hpp"
#include "IGameModule.hpp"
#include "DLLoader.hpp"

namespace arcade {
    class Core {
        public:
            Core() = default;
            ~Core() = default;
            void run(const std::string &lib);
        private:
            std::vector<std::string> _libs;
            DLLoader<IDisplayModule> _loaderGraphic;
            DLLoader<IGameModule> _loaderGame;
            void *_handleGraphic = nullptr;
            void *_handleGame = nullptr;
            std::shared_ptr<IDisplayModule> _display = nullptr;
            std::unique_ptr<IGameModule> _game = nullptr;
            EType _type;
            std::string _name;
            std::string _currentGraphicPath;
            std::string _currentGamePath;
            void loadGraphic(const std::string &path);
            std::string loadGame(const std::string &path);
            std::string changeDisplay();
            std::vector<std::string> initLibList(std::string lib);
            std::string runGames(IRect &selector, bool &running);
            std::string runlib(IRect &selector, bool &running);
    };
}
