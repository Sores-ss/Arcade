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
    const EType Core::getType(const std::string &lib)
    {
        DLLoader<IDisplayModule> loader;

        void *handle = loader.open(lib);
        if (!handle)
            throw Exception("Invalid open library");
        IDisplayModule *library = loader.getInstance(handle);
        EType type = library->getType();
        loader.close(handle);
        return type;
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
}