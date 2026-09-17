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
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_image.h>
#include <algorithm>


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
    void SDL2::init(std::string name, Size size) {
        SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
        IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);
        TTF_Init();

        _windowSize = {size.w, size.h};
        SDL_Window* window = SDL_CreateWindow(
            name.c_str(),
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            size.w, size.h,
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

    void SDL2::setBorder(IRect &rect, Texture texture)
    {
        SDLRect *sdlrect = dynamic_cast<SDLRect *>(&rect);
        if (!sdlrect)
            return;
        sdlrect->setBorderRect(sdlrect->getRect());
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
            sdlrect->setBorderTexture(surf_texture);
            _textures.push_back(surf_texture);
        } else {
            sdlrect->setBorderColor((SDL_Color){texture.r, texture.g, texture.b, texture.a});
        }
    }

    void SDL2::stop() {
        if (!_textures.empty())
            for (auto texture : _textures)
                SDL_DestroyTexture(texture);
        if (_music)
            Mix_FreeMusic(_music);
        if (_renderer)
            SDL_DestroyRenderer(_renderer);
        if (_window)
            SDL_DestroyWindow(_window);
        TTF_Quit();
        IMG_Quit();
        SDL_Quit();
    }

    void SDL2::setMusic(std::string filepath) {
        if (_music) {
            Mix_FreeMusic(_music);
            _music = nullptr;
        }
        Mix_Music* music = Mix_LoadMUS(filepath.c_str());

        if (!music) {
            std::cout << "Error loading music" << std::endl;
            return;
        }
        _music = music;
        Mix_PlayMusic(music, -1);
    }

    void SDL2::playSound(std::string filepath) const {
        Mix_Chunk *sound = Mix_LoadWAV(filepath.c_str());

        if (!sound) {
            std::cout << "Error loading sound" << std::endl;
            return;
        }

        Mix_PlayChannel(-1, sound, 0);
        Mix_FreeChunk(sound);
    }

    const EType SDL2::getType() const {
        return _type;
    }

    const std::string SDL2::getName() const {
        static const std::string name = "SDL2";
        return name;
    }

    void SDL2::displayRect(IRect &r) const {
        SDL2::SDLRect *rectObj = dynamic_cast<SDL2::SDLRect *>(&r);

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
        if (rectObj->hasBorder()) {
            if (rectObj->getBorderTexture())
                SDL_RenderCopy(_renderer, rectObj->getBorderTexture(), NULL, &rectObj->getBorder());
            else {
                SDL_SetRenderDrawColor(_renderer,
                    rectObj->getBorderColor().r,
                    rectObj->getBorderColor().g,
                    rectObj->getBorderColor().b,
                    rectObj->getBorderColor().a);
                SDL_RenderDrawRect(_renderer, &rectObj->getBorder());
            }
        }
    }

    IRect *SDL2::createRect(Bounds bound) {
        SDL_Rect rect = {
            static_cast<int>(bound.x),
            static_cast<int>(bound.y),
            static_cast<int>(bound.w),
            static_cast<int>(bound.h)
        };
        SDL_Color color = {255, 255, 255, 255};

        return new SDLRect(rect, color);
    }

    void SDL2::SDLRect::setSize(Size size) {
        _rect.h = size.h;
        _rect.w = size.w;
    }

    Bounds SDL2::SDLRect::getBounds() const {
        return {{static_cast<std::size_t>(_rect.x), static_cast<std::size_t>(_rect.y)}, {static_cast<std::size_t>(_rect.w), static_cast<std::size_t>(_rect.h)}};
    }

    bool SDL2::SDLRect::isMouseOver() const
    {
        int mouseX = 0;
        int mouseY = 0;

        SDL_GetMouseState(&mouseX, &mouseY);
        return mouseX >= _rect.x && mouseX < _rect.x + _rect.w
            && mouseY >= _rect.y && mouseY < _rect.y + _rect.h;
    }

    void SDL2::SDLRect::setPosition(Position pos)
    {
        _rect.x = pos.x;
        _rect.y = pos.y;
        _textRect.x = _rect.x + (_rect.w - _textRect.w) / 2;
        _textRect.y = _rect.y + (_rect.h - _textRect.h) / 2;
        _border.x = pos.x;
        _border.y = pos.y;
    }

    void SDL2::setBackground(Texture texture)
    {
        if (texture.filepath != "") {
        SDL_Surface* surface = IMG_Load(texture.filepath.c_str());
        if (!surface) {
            std::cout << "Erreur chargement image: " << IMG_GetError() << std::endl;
            return;
        }

        SDL_Texture* texture = SDL_CreateTextureFromSurface(_renderer, surface);
        if (!texture) {
            std::cout << "Erreur création texture: " << SDL_GetError() << std::endl;
            SDL_FreeSurface(surface);
            return;
        }
        SDL_FreeSurface(surface);
        _background = texture;
        _textures.push_back(texture);
        }
    }

    void SDL2::setText(IRect &rectObj, std::string text, Texture texture)
    {
        TTF_Font* font = TTF_OpenFont(texture.filepath.c_str(), 24);
        if (!font) {
            std::cerr << "Erreur font: " << TTF_GetError() << std::endl;
            return;
        }

        SDL_Color color = {255, 255, 255, 255};

        SDL_Surface* textSurface = TTF_RenderText_Solid(font, text.c_str(), color);
        if (!textSurface) {
            std::cerr << "Erreur rendu texte: " << TTF_GetError() << std::endl;
            TTF_CloseFont(font);
            return;
        }

        SDL_Texture* textTexture = SDL_CreateTextureFromSurface(_renderer, textSurface);
        SDL_Rect textRect;

        SDLRect *sdlRect = dynamic_cast<SDLRect *>(&rectObj);
        if (!sdlRect) {
            SDL_FreeSurface(textSurface);
            TTF_CloseFont(font);
            return;
        }
        SDL_Rect rect = sdlRect->getRect();

        const float maxWidth = static_cast<float>(rect.w) * 0.85f;
        const float maxHeight = static_cast<float>(rect.h) * 0.85f;
        const float scaleX = maxWidth / static_cast<float>(textSurface->w);
        const float scaleY = maxHeight / static_cast<float>(textSurface->h);
        const float scale = std::min(scaleX, scaleY);

        textRect.w = static_cast<int>(textSurface->w * scale);
        textRect.h = static_cast<int>(textSurface->h * scale);
        textRect.x = rect.x + (rect.w - textRect.w) / 2;
        textRect.y = rect.y + (rect.h - textRect.h) / 2;
        SDL_FreeSurface(textSurface);
        TTF_CloseFont(font);
        sdlRect->setTextRect(textRect);
        sdlRect->setTextTexture(textTexture);
        _textures.push_back(textTexture);
    }

    void SDL2::setTexture(IRect &rect, Texture texture)
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
            SDLRect *sdl_rect = dynamic_cast<SDLRect *>(&rect);
            if (!sdl_rect)
                return;
            sdl_rect->setTexture(surf_texture);
            _textures.push_back(surf_texture);
        } else {
            SDLRect *sdl_rect = dynamic_cast<SDLRect *>(&rect);
            if (!sdl_rect)
                return;
            sdl_rect->setColor((SDL_Color){texture.r, texture.g, texture.b, texture.a});
        }
    }

    void SDL2::clearWindow() const
    {
        SDL_SetRenderDrawColor(_renderer, _backgroundColor.r, _backgroundColor.g, _backgroundColor.b, _backgroundColor.a);
        SDL_RenderClear(_renderer);
        if (_background)
            SDL_RenderCopy(_renderer, _background, NULL, NULL);
    }

    void SDL2::render()
    {
        SDL_RenderPresent(_renderer);
    }

    const EEvent SDL2::pollEvent()
    {
        while (SDL_PollEvent(&_event)) {
            if (_event.type == SDL_QUIT)
                return EEvent::QUIT;
            if (_event.type == SDL_KEYDOWN) {
                if (_event.key.keysym.sym == SDLK_ESCAPE)
                    return EEvent::ESCAPE;
                if (_event.key.keysym.sym == SDLK_DOWN)
                    return EEvent::DOWN;
                if (_event.key.keysym.sym == SDLK_UP)
                    return EEvent::UP;
                if (_event.key.keysym.sym == SDLK_LEFT)
                    return EEvent::LEFT;
                if (_event.key.keysym.sym == SDLK_RIGHT)
                    return EEvent::RIGHT;
                if (_event.key.keysym.sym == SDLK_RETURN)
                    return EEvent::ENTER;
            }
        }
        return EEvent::UNDEFINED;
    }
}
