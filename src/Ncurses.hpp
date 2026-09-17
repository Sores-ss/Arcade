/*
** EPITECH PROJECT, 2025
** G-OOP-400-LIL-4-1-arcade-9
** File description:
** Ncurses.hpp
*/

#pragma once

#include "IDisplayModule.hpp"
#include <ncurses.h>

namespace arcade {
    class Ncurses : public IDisplayModule {
        protected:
            Size _windowSize;
            MEVENT _event;
            int _colors_id = 1;
            int _bkdg_pair;
            int _bkdg_color;
            int _pairs_id = 1;
        public:
            class ncursesRect : public IRect {
                protected:
                    Bounds _bounds;
                    WINDOW *_win;
                    int &_colors_id;
                    int &_pairs_id;
                    MEVENT &_event;
                    std::string _text = "";
                    bool _hasBkdg = false;
                    bool _hasBorder = false;
                    int _borderColor_id;
                    int _borderPair_id;
                    int _textColor_id;
                    int _pair_id;
                    int _color_id;
                public:
                    ncursesRect(int &colors_id, int &pairs_id, MEVENT &event) : _colors_id(colors_id), _pairs_id(pairs_id), _event(event){};
                    ~ncursesRect() {delwin(_win);};
                    void setWin(WINDOW *win) {_win = win;}
                    Bounds getBounds() const override;
                    bool isMouseOver() const override;
                    void setSize(Size size) override;
                    void display() const override;
                    void setPosition(Position position) override;
                    void setText(std::string text, Texture texture) override;
                    void setBorder(Texture texture) override;
                    void setTexture(Texture texture) override;
            };
            Ncurses() = default;
            ~Ncurses() = default;
            void init(std::string name, Size size) override;
            void stop() override;
            void setBackground(Texture texture) override;
            IRect *createRect(Bounds bounds) override;
            void setMusic(std::string filepath) override;
            void playSound(std::string filepath) const override;
            void render() override;
            EEvent pollEvent() override;
            void clearWindow() const override;
            const Size getWindowSize() const override {return _windowSize;}
    };
}