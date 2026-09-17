/*
** EPITECH PROJECT, 2025
** G-OOP-400-LIL-4-1-arcade-9
** File description:
** arcarde.c
*/

#include "DLLoader.hpp"
#include "Core.hpp"
#include "IDisplayModule.hpp"
#include <vector>

int main(int argc, char **argv)
{
    if (argc < 2) {
        std::cerr << "Invalid number of argument" << std::endl;
        return 84;
    }
    try {
        std::vector<std::string> libraries = arcade::Core::initLibList(argv[1]);
        for (const std::string &lib : libraries)
            std::cout << lib << std::endl;
    } catch(const std::exception& e) {
        std::cerr << e.what() << '\n';
        return 84;
    }
    return 0;
}