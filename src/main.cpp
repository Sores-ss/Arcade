/*
** EPITECH PROJECT, 2025
** G-OOP-400-LIL-4-1-arcade-9
** File description:
** arcarde.c
*/

#include "DLLoader.hpp"
#include "Core.hpp"
#include "IDisplayModule.hpp"
#include <map>

int main(void)
{
    std::map<arcade::EType, std::string> libraries = arcade::Core::initLibList();
    return 0;
}