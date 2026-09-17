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

__attribute__((destructor)) void destroy(void)
{
    std::cout << "Closing libsdl2..." << std::endl;
}

extern "C" {
    arcade::SDL2 *myEntryPoint(void)
    {
        std::cout << "Loading libsdl2.." << std::endl;
        return new arcade::SDL2();
    }

    const arcade::EType getLibType(void)
    {
        return arcade::EType::GRAPHICAL;
    }

    const std::string getLibName(void)
    {
        return "SDL2";
    }
}

namespace arcade {
    void SDL2::init(std::string name, size_t width, size_t height) {
        SDL_Window* window = SDL_CreateWindow(
            name.c_str(),
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            width, height,
            SDL_WINDOW_SHOWN
        );
        if (!window) {
            std::cerr << "Erreur création fenêtre : " << SDL_GetError() << std::endl;
            SDL_Quit();
            return;
        }
        _window = window;
        SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        if (!renderer) {
            std::cout << "GPU indisponible, fallback CPU..." << std::endl;
            renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
            if (!renderer) {
                std::cerr << "Erreur renderer : " << SDL_GetError() << std::endl;
                SDL_DestroyWindow(_window);
                SDL_Quit();
                return;
            }
        }
        _renderer = renderer;
    }

    void SDL2::stop() {
        if (_renderer)
            SDL_DestroyRenderer(_renderer);
        if (_window)
            SDL_DestroyWindow(_window);
        SDL_Quit();
    }

    const void SDL2::display(IButton *b) const {
        SDL2::SDLButton *button = dynamic_cast<SDL2::SDLButton *>(b);

        if (!button || !_renderer)
            return;
        SDL_Rect rect = button->getRect();
        SDL_SetRenderDrawColor(_renderer,
            button->getColor().r,
            button->getColor().g,
            button->getColor().b,
            button->getColor().a);
        SDL_RenderFillRect(_renderer, &rect);
    }

    const IButton *SDL2::createButton(size_t width, size_t height, size_t x, size_t y) {
        SDL_Rect rect = {
            static_cast<int>(x),
            static_cast<int>(y),
            static_cast<int>(width),
            static_cast<int>(height)
        };
        SDL_Color color = {255, 255, 255, 255};

        return new SDLButton(rect, color);
    }

    bool SDL2::SDLButton::isMouseOver() const
    {
        int mouseX = 0;
        int mouseY = 0;

        SDL_GetMouseState(&mouseX, &mouseY);
        return mouseX >= _rect.x && mouseX < _rect.x + _rect.w
            && mouseY >= _rect.y && mouseY < _rect.y + _rect.h;
    }

    void SDL2::SDLButton::setText(std::string text) const
    {
        (void)text;
    }

    void SDL2::render()
    {
        SDL_RenderPresent(_renderer);
    }

    EEvent SDL2::pollEvent()
    {
        while (SDL_PollEvent(&_event)) {
            if (_event.type == SDL_QUIT)
                return EEvent::QUIT;
            if (_event.type == SDL_KEYDOWN) {
                if (_event.key.keysym.sym == SDLK_ESCAPE)
                    return EEvent::ESCAPE;
            }
        }
        return EEvent::UNDEFINED;
    }
}
