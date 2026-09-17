/*
** EPITECH PROJECT, 2025
** Visual Studio Live Share (Workspace)
** File description:
** SDL2.cpp
*/

#include "SDL2.hpp"
#include "Exception.hpp"
#include <SDL2/SDL.h>


__attribute__((constructor)) void create(void)
{
    std::cout << "Opening libsdl2..." << std::endl;
}

__attribute((destructor)) void destroy(void)
{
    std::cout << "Closing libsdl2..." << std::endl;
}


extern "C" arcade::SDL2 *myEntryPoint(void)
{
    std::cout << "Loading libsdl2.." << std::endl;
    return new arcade::SDL2();
}

namespace arcade {
    void SDL2::init() {
        if (SDL_Init(SDL_INIT_VIDEO) != 0) {
            std::cerr << "Erreur SDL_Init : " << SDL_GetError() << std::endl;
            return;
        }

        // Création de la fenêtre
        SDL_Window* window = SDL_CreateWindow(
            "Ma Fenêtre SDL2",
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            800, 600,
            SDL_WINDOW_SHOWN
        );

        if (!window) {
            std::cerr << "Erreur création fenêtre : " << SDL_GetError() << std::endl;
            SDL_Quit();
            return;
        }

        // Boucle principale simple
        bool running = true;
        SDL_Event event;

        while (running) {
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT) {
                    running = false;
                }
            }

            // Petite pause pour éviter de cramer le CPU
            SDL_Delay(16);
        }

        // Nettoyage
        SDL_DestroyWindow(window);
        SDL_Quit();
    }

    void SDL2::stop() {
        return;
    }

    const EType &SDL2::getType() const {
        return _type;
    }

    const std::string SDL2::getName() const {
        return "SDL2";
    }

    IButton *SDL2::createButton(size_t width, size_t height, size_t x, size_t y) {
        return new IButton();
    }
}
