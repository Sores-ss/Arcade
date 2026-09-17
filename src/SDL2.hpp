/*
** EPITECH PROJECT, 2025
** Visual Studio Live Share (Workspace)
** File description:
** SDL2.hpp
*/

#pragma once

#include "IDisplayModule.hpp"
#include <SDL2/SDL.h>
#include <vector>

namespace arcade {
    class SDL2 : public IDisplayModule {
        protected:
            EType _type = GRAPHICAL;
            SDL_Window *_window = nullptr;
            SDL_Renderer *_renderer = nullptr;
            std::vector<SDL_Texture *> _textures;
            SDL_Event _event;
        public:
            class SDLRect : public IRect {
                protected:
                    SDL_Rect _rect;
                    SDL_Texture *_texture = nullptr;
                    SDL_Texture *_textTexture = nullptr;
                    SDL_Rect _textRect;
                    SDL_Color _color;
                public:
                    SDLRect(SDL_Rect rect, SDL_Color color) {_rect = rect; _color = color;}
                    ~SDLRect() = default;
                    SDL_Texture *getTextTexture() const {return _textTexture;}
                    SDL_Rect &getTextRect() {return _textRect;}
                    void setTexture(SDL_Texture *texture) {_texture = texture;}
                    SDL_Texture *getTexture() const {return _texture;}
                    bool isMouseOver() const override;
                    void setColor(SDL_Color color) {_color = color;}
                    SDL_Color &getColor() {return _color;}
                    SDL_Rect &getRect() {return _rect;}
                    void setPosition(std::size_t x, std::size_t y) override;
            };
            SDL2() = default;
            ~SDL2() = default;
            void clear() const override;
            void init(std::string, size_t, size_t) override;
            void stop() override;
            virtual void render() override;
            virtual EEvent pollEvent() override;
            void displayRect(IRect *b) const override;
            void setTexture(IRect *rect, Texture_t texture) override;
            void setText(IRect *rect, std::string text, Texture_t texture) override;
            const std::string &getName() const override;
            const EType &getType() const override;
            const IRect *createRect(size_t width, size_t height, size_t x, size_t y) override;
    };
}
