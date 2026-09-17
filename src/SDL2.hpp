/*
** EPITECH PROJECT, 2025
** Visual Studio Live Share (Workspace)
** File description:
** SDL2.hpp
*/

#pragma once

#include "IDisplayModule.hpp"
#include <SDL2/SDL.h>

namespace arcade {
    class SDL2 : public IDisplayModule {
        protected:
            EType _type = GRAPHICAL;
            SDL_Window *_window = nullptr;
            SDL_Renderer *_renderer = nullptr;
            SDL_Event _event;
        public:
            class SDLButton : public IButton {
                protected:
                    SDL_Rect _rect;
                    SDL_Color _color;
                public:
                    SDLButton(SDL_Rect rect, SDL_Color color) {_rect = rect; _color = color;}
                    ~SDLButton() = default;
                    bool isMouseOver() const override;
                    SDL_Color getColor() const {return _color;}
                    SDL_Rect getRect() const {return _rect;}
                    void setText(std::string text) const override;
            };
            SDL2() = default;
            ~SDL2() = default;
            void init(std::string, size_t, size_t) override;
            void stop() override;
            virtual void render() override;
            virtual EEvent pollEvent() override;
            const void display(IButton *b) const override;
            const IButton *createButton(size_t width, size_t height, size_t x, size_t y) override;
    };
}
