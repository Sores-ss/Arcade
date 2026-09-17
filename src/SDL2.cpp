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
    return;
}

__attribute__((destructor)) void destroy(void)
{
    return;
}

extern "C" {
    arcade::SDL2 *myEntryPoint(void)
    {
        return new arcade::SDL2();
    }

    arcade::EType getLibType(void)
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
        Mix_Init(MIX_INIT_MP3 | MIX_INIT_OGG | MIX_INIT_WAVPACK);

        if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 512) < 0) {
            std::cerr << "Erreur initialisation audio: " << Mix_GetError() << std::endl;
            _audioEnabled = false;
        } else
            _audioEnabled = true;
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

    void SDL2::SDLRect::setBorder(Texture texture)
    {
        setBorderRect(_rect);
        if (_borderTexture) {
            SDL_DestroyTexture(_borderTexture);
            _borderTexture = nullptr;
        }
        if (texture.filepath != "") {
            SDL_Surface *surface = IMG_Load(texture.filepath.c_str());
            if (!surface) {
                std::cerr << "Erreur IMG_Load: " << IMG_GetError() << std::endl;
                return;
            }
            SDL_Texture* surf_texture = SDL_CreateTextureFromSurface(&_renderer, surface);
            SDL_FreeSurface(surface);

            if (!surf_texture) {
                std::cerr << "Erreur texture: " << SDL_GetError() << std::endl;
                return;
            }
            setBorderTexture(surf_texture);
        } else
            setBorderColor((SDL_Color){texture.r, texture.g, texture.b, texture.a});
    }

    void SDL2::stop() {
        if (_background) {
            SDL_DestroyTexture(_background);
            _background = nullptr;
        }
        _textures.clear();
        for (auto &sound : _sounds)
            Mix_FreeChunk(sound.second);
        _sounds.clear();
        if (_music)
            Mix_FreeMusic(_music);
        Mix_CloseAudio();
        Mix_Quit();
        if (_renderer)
            SDL_DestroyRenderer(_renderer);
        if (_window)
            SDL_DestroyWindow(_window);
        TTF_Quit();
        IMG_Quit();
        SDL_Quit();
    }

    void SDL2::setMusic(std::string filepath) {
        if (!_audioEnabled)
            return;
        if (_music) {
            Mix_FreeMusic(_music);
            _music = nullptr;
        }
        Mix_Music* music = Mix_LoadMUS(filepath.c_str());

        if (!music) {
            std::cout << "Error loading music: " << Mix_GetError() << std::endl;
            return;
        }
        _music = music;
        if (Mix_PlayMusic(music, -1) == -1)
            std::cout << "Error playing music: " << Mix_GetError() << std::endl;
    }

    void SDL2::playSound(std::string filepath) {
        if (!_audioEnabled)
            return;
        Mix_Chunk *sound = nullptr;
        auto it = _sounds.find(filepath);

        if (it == _sounds.end()) {
            sound = Mix_LoadWAV(filepath.c_str());
            if (!sound) {
                std::cout << "Error loading sound: " << Mix_GetError() << std::endl;
                return;
            }
            _sounds[filepath] = sound;
        } else
            sound = it->second;

        if (Mix_PlayChannel(-1, sound, 0) == -1)
            std::cout << "Error playing sound: " << Mix_GetError() << std::endl;
    }

    void SDL2::SDLRect::display() const {
        if (_texture)
            SDL_RenderCopy(&_renderer, _texture, NULL, &_rect);
        else {
            SDL_SetRenderDrawColor(&_renderer,
                _color.r,
                _color.g,
                _color.b,
                _color.a);
            SDL_RenderFillRect(&_renderer, &_rect);
        }
        if (_textTexture) {
            SDL_RenderCopy(&_renderer, _textTexture, NULL, &_textRect);
        }
        if (_hasBorder) {
            if (_borderTexture)
                SDL_RenderCopy(&_renderer, _borderTexture, NULL, &_border);
            else {
                SDL_SetRenderDrawColor(&_renderer,
                    _borderColor.r,
                    _borderColor.g,
                    _borderColor.b,
                    _borderColor.a);
                SDL_RenderDrawRect(&_renderer, &_border);
            }
        }
    }

    std::shared_ptr<IRect> SDL2::createRect(Bounds bound) {
        SDL_Rect rect = {
            static_cast<int>(bound.x),
            static_cast<int>(bound.y),
            static_cast<int>(bound.w),
            static_cast<int>(bound.h)
        };
        SDL_Color color = {255, 255, 255, 255};

        return std::make_shared<SDLRect>(rect, color, *_renderer);
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
        if (_background) {
            SDL_DestroyTexture(_background);
            _background = nullptr;
        }
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
        } else {
            _backgroundColor = {texture.r, texture.g, texture.b, texture.a};
        }
    }

    void SDL2::SDLRect::setText(std::string text, Texture texture)
    {
        if (_textTexture)
            SDL_DestroyTexture(_textTexture);
        _textTexture = nullptr;
        if (text.empty()) {
            _textRect = {0, 0, 0, 0};
            return;
        }
        TTF_Font *font = nullptr;
        if (!texture.filepath.empty())
            font = TTF_OpenFont(texture.filepath.c_str(), 24);
        if (!font)
            font = TTF_OpenFont("./assets/font/font.ttf", 24);
        if (!font) {
            static bool fontErrorPrinted = false;
            if (!fontErrorPrinted) {
                std::cerr << "Erreur font: " << TTF_GetError() << std::endl;
                fontErrorPrinted = true;
            }
            return;
        }

        SDL_Color color = {255, 255, 255, 255};

        SDL_Surface* textSurface = TTF_RenderText_Solid(font, text.c_str(), color);
        if (!textSurface) {
            std::cerr << "Erreur rendu texte: " << TTF_GetError() << std::endl;
            TTF_CloseFont(font);
            return;
        }

        SDL_Texture* textTexture = SDL_CreateTextureFromSurface(&_renderer, textSurface);
        SDL_Rect textRect;

        SDL_Rect rect = _rect;

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
        setTextRect(textRect);
        setTextTexture(textTexture);
    }

    void SDL2::SDLRect::setTexture(Texture texture)
    {
        if (_texture) {
            SDL_DestroyTexture(_texture);
            _texture = nullptr;
        }
        if (texture.filepath != "") {
            SDL_Surface *surface = IMG_Load(texture.filepath.c_str());
            if (!surface) {
                std::cerr << "Erreur IMG_Load: " << IMG_GetError() << std::endl;
                return;
            }
            SDL_Texture* surf_texture = SDL_CreateTextureFromSurface(&_renderer, surface);
            SDL_FreeSurface(surface);

            if (!surf_texture) {
                std::cerr << "Erreur texture: " << SDL_GetError() << std::endl;
                return;
            }
            setTexture(surf_texture);
        } else
            setColor((SDL_Color){texture.r, texture.g, texture.b, texture.a});
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

    EEvent SDL2::pollEvent()
    {
        while (SDL_PollEvent(&_event)) {
            if (_event.type == SDL_QUIT)
                return EEvent::QUIT;
            if (_event.type == SDL_MOUSEBUTTONDOWN)
                return EEvent::CLICK;
            if (_event.type == SDL_KEYDOWN) {
                if (_event.key.keysym.sym == SDLK_F1)
                    return EEvent::F1;
                if (_event.key.keysym.sym == SDLK_F5)
                    return EEvent::F5;
                if (_event.key.keysym.sym == SDLK_DELETE)
                    return EEvent::SUPPR;
                if (_event.key.keysym.sym == SDLK_ESCAPE)
                    return EEvent::ESCAPE;
                if (_event.key.keysym.sym == SDLK_BACKSPACE)
                    return EEvent::BACKSPACE;
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
                if (_event.key.keysym.sym == SDLK_TAB)
                    return EEvent::TAB;
                if (_event.key.keysym.sym >= SDLK_a && _event.key.keysym.sym <= SDLK_z)
                    return static_cast<EEvent>(EEvent::A + (_event.key.keysym.sym - SDLK_a));
            }
        }
        return EEvent::UNDEFINED;
    }
}
