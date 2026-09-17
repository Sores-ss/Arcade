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
            IDisplayModule *_display = nullptr;
            IGameModule *_game = nullptr;
            EType _type;
            std::string _name;
            void loadGraphic(const std::string &path);
            void loadGame(const std::string &path);
            std::vector<std::string> initLibList(std::string lib);
            void runGame(const std::string path);
    };
}
