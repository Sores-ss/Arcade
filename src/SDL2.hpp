/*
** EPITECH PROJECT, 2025
** Visual Studio Live Share (Workspace)
** File description:
** SDL2.hpp
*/

#pragma once

#include "IDisplayModule.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <vector>

namespace arcade {
    class SDL2 : public IDisplayModule {
        protected:
            EType _type = GRAPHICAL;
            Mix_Music *_music = nullptr;
            SDL_Texture *_background = nullptr;
            SDL_Color _backgroundColor = {30, 30, 30, 255};
            SDL_Window *_window = nullptr;
            SDL_Renderer *_renderer = nullptr;
            std::vector<SDL_Texture *> _textures;
            SDL_Event _event;
            Size _windowSize;
        public:
            class SDLRect : public IRect {
                protected:
                    SDL_Rect _rect;
                    SDL_Rect _border;
                    bool _hasBorder = false;
                    SDL_Texture *_borderTexture = nullptr;
                    SDL_Color _borderColor;
                    SDL_Texture *_texture = nullptr;
                    SDL_Texture *_textTexture = nullptr;
                    SDL_Rect _textRect;
                    SDL_Color _color;
                public:
                    SDLRect(SDL_Rect rect, SDL_Color color) {_rect = rect; _color = color;}
                    ~SDLRect() = default;
                    Bounds getBounds() const override;
                    SDL_Texture *getTextTexture() const {return _textTexture;}
                    void setSize(Size size);
                    SDL_Rect &getTextRect() {return _textRect;}
                    SDL_Rect &getBorder() {return _border;}
                    SDL_Color getBorderColor() {return _borderColor;}
                    void setBorderColor(SDL_Color color) {_borderColor = color;}
                    SDL_Texture *getBorderTexture() {return _borderTexture;}
                    bool hasBorder() {return _hasBorder;}
                    void display() const override {return;}
                    void setBorderTexture(SDL_Texture *texture) { _borderTexture = texture;}
                    void setBorderRect(SDL_Rect r) {_border = r; _hasBorder = true;}
                    void setTexture(SDL_Texture *texture) {_texture = texture;}
                    void setTextRect(SDL_Rect rect) {_textRect = rect;}
                    void setTextTexture(SDL_Texture *textTexture) {_textTexture = textTexture;}
                    SDL_Texture *getTexture() const {return _texture;}
                    bool isMouseOver() const override;
                    void setColor(SDL_Color color) {_color = color;}
                    SDL_Color &getColor() {return _color;}
                    SDL_Rect &getRect() {return _rect;}
                    void setPosition(Position pos) override;
            };
            SDL2() = default;
            ~SDL2() = default;
            void setMusic(std::string filepath) override;
            void playSound(std::string filepath) const override;
            const Size getWindowSize() const override {return _windowSize;}
            void setBackground(Texture texture) override;
            void clearWindow() const override;
            void init(std::string, Size size) override;
            void stop() override;
            void render() override;
            const EEvent pollEvent() override;
            void setBorder(IRect &rect, Texture texture) override;
            void displayRect(IRect &b) const override;
            void setTexture(IRect &rect, Texture texture) override;
            void setText(IRect &rect, std::string text, Texture texture) override;
            const std::string getName() const override;
            const EType getType() const override;
            IRect *createRect(Bounds bound) override;
    };
}
