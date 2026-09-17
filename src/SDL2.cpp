/*
** EPITECH PROJECT, 2025
** Visual Studio Live Share (Workspace)
** File description:
** SDL2.cpp
*/

#include "SDL2.hpp"
#include "Exception.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>


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
        SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
        IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);
        TTF_Init();
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
        if (!_textures.empty())
            for (auto texture : _textures)
                SDL_DestroyTexture(texture);
        if (_renderer)
            SDL_DestroyRenderer(_renderer);
        if (_window)
            SDL_DestroyWindow(_window);
        TTF_Quit();
        IMG_Quit();
        SDL_Quit();
    }

    const EType &SDL2::getType() const {
        return _type;
    }

    const std::string &SDL2::getName() const {
        static const std::string name = "SDL2";
        return name;
    }

    void SDL2::displayRect(IRect *r) const {
        SDL2::SDLRect *rectObj = dynamic_cast<SDL2::SDLRect *>(r);

        if (!rectObj || !_renderer)
            return;
        SDL_Rect rect = rectObj->getRect();
        if (rectObj->getTexture())
            SDL_RenderCopy(_renderer, rectObj->getTexture(), NULL, &rect);
        else {
            SDL_SetRenderDrawColor(_renderer,
                rectObj->getColor().r,
                rectObj->getColor().g,
                rectObj->getColor().b,
                rectObj->getColor().a);
            SDL_RenderFillRect(_renderer, &rect);
        }
        if (rectObj->getTextTexture()) {
            SDL_RenderCopy(_renderer, rectObj->getTextTexture(), NULL, &rectObj->getTextRect());
        }
    }

    IRect *SDL2::createRect(size_t width, size_t height, size_t x, size_t y) {
        SDL_Rect rect = {
            static_cast<int>(x),
            static_cast<int>(y),
            static_cast<int>(width),
            static_cast<int>(height)
        };
        SDL_Color color = {255, 255, 255, 255};

        return new SDLRect(rect, color);
    }

    bool SDL2::SDLRect::isMouseOver() const
    {
        int mouseX = 0;
        int mouseY = 0;

        SDL_GetMouseState(&mouseX, &mouseY);
        return mouseX >= _rect.x && mouseX < _rect.x + _rect.w
            && mouseY >= _rect.y && mouseY < _rect.y + _rect.h;
    }

    void SDL2::SDLRect::setPosition(size_t x, size_t y)
    {
        _rect.x = x;
        _rect.y = y;
        _textRect.x = _rect.x + (_rect.w - _textRect.w) / 2;
        _textRect.y = _rect.y + (_rect.h - _textRect.h) / 2;
    }

    void SDL2::setText(IRect *rectObj, std::string text, Texture_t texture)
    {
        TTF_Font* font = TTF_OpenFont(texture.filepath.c_str(), 24);
        if (!font) {
            std::cerr << "Erreur font: " << TTF_GetError() << std::endl;
            return;
        }

        SDL_Color color = {255, 255, 255, 255};

        SDL_Surface* textSurface = TTF_RenderText_Solid(font, text.c_str(), color);

        SDL_Texture* textTexture = SDL_CreateTextureFromSurface(_renderer, textSurface);
        SDL_Rect textRect;
        textRect.w = textSurface->w * 3;
        textRect.h = textSurface->h * 3;

        SDLRect *sdlRect = dynamic_cast<SDLRect *>(rectObj);
        if (!sdlRect)
            return;
        SDL_Rect rect = sdlRect->getRect();
        textRect.x = rect.x + (rect.w - textRect.w) / 2;
        textRect.y = rect.y + (rect.h - textRect.h) / 2;
        SDL_FreeSurface(textSurface);
        sdlRect->setTextRect(textRect);
        sdlRect->setTextTexture(textTexture);
        _textures.push_back(textTexture);
    }

    void SDL2::setTexture(IRect *rect, Texture_t texture)
    {
        if (texture.filepath != "") {
            SDL_Surface *surface = IMG_Load(texture.filepath.c_str());
            if (!surface) {
                std::cerr << "Erreur IMG_Load: " << IMG_GetError() << std::endl;
                return;
            }
            SDL_Texture* surf_texture = SDL_CreateTextureFromSurface(_renderer, surface);
            SDL_FreeSurface(surface);

            if (!surf_texture) {
                std::cerr << "Erreur texture: " << SDL_GetError() << std::endl;
                return;
            }
            SDLRect *sdl_rect = dynamic_cast<SDLRect *>(rect);
            if (!sdl_rect)
                return;
            sdl_rect->setTexture(surf_texture);
            _textures.push_back(surf_texture);
        } else {
            SDLRect *sdl_rect = dynamic_cast<SDLRect *>(rect);
            if (!sdl_rect)
                return;
            sdl_rect->setColor((SDL_Color){texture.r, texture.g, texture.b, texture.a});
        }
    }

    void SDL2::clear() const
    {
        SDL_SetRenderDrawColor(_renderer, 30, 30, 30, 255);
        SDL_RenderClear(_renderer);
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
