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
#include "DLLoader.hpp"

namespace arcade {
    class Core {
        public:
            Core() = default;
            ~Core() = default;
            static std::vector<std::string> initLibList(std::string lib);
            void run(const std::string &lib);
        private:
            std::vector<std::string> _libs;
            DLLoader<IDisplayModule> _loader;
            void *_handle = nullptr;
            IDisplayModule *_display = nullptr;
            EType _type;
            std::string _name;
            void loadLib(const std::string &path);
    };
}