/*
** EPITECH PROJECT, 2025
** Visual Studio Live Share (Workspace)
** File description:
** Ncurses.cpp
*/

#include "Ncurses.hpp"
#include "Exception.hpp"
#include <algorithm>

__attribute__((constructor)) void create(void)
{
    std::cout << "Opening libNcurses..." << std::endl;
}

__attribute__((destructor)) void destroy(void)
{
    std::cout << "Closing libNcurses..." << std::endl;
}

extern "C" {
    arcade::Ncurses *myEntryPoint(void)
    {
        std::cout << "Loading libNcurses.." << std::endl;
        return new arcade::Ncurses();
    }

    arcade::EType getLibType(void)
    {
        return arcade::EType::GRAPHICAL;
    }

    const std::string getLibName(void)
    {
        return "Ncurses";
    }
}

namespace arcade {
    short Ncurses::textureToColor(const arcade::Texture &texture)
    {
        if (texture.r > 220 && texture.g > 220 && texture.b > 220)
            return COLOR_WHITE;
        if (texture.r > 220 && texture.g > 220)
            return COLOR_YELLOW;
        if (texture.r > 220 && texture.b > 220)
            return COLOR_MAGENTA;
        if (texture.g > 220 && texture.b > 220)
            return COLOR_CYAN;
        if (texture.r > texture.g && texture.r > texture.b)
            return COLOR_RED;
        if (texture.g > texture.r && texture.g > texture.b)
            return COLOR_GREEN;
        if (texture.b > texture.r && texture.b > texture.g)
            return COLOR_BLUE;
        return COLOR_BLACK;
    }

    int Ncurses::pairFor(int fg, int bg)
    {
        int clampedFg = std::max(0, std::min(fg, 7));
        int bgIndex = (bg < 0) ? 0 : std::max(1, std::min(bg + 1, 8));
        int pairId = 1 + clampedFg * 9 + bgIndex;

        init_pair(pairId, clampedFg, (bg < 0) ? -1 : bg);
        return pairId;
    }
    
    void Ncurses::init([[maybe_unused]]std::string name, [[maybe_unused]]Size size) {
        initscr();
        if (has_colors())
            start_color();
        use_default_colors();
        cbreak();
        noecho();
        curs_set(0);
        timeout(16);
        keypad(stdscr, TRUE);
        mousemask(ALL_MOUSE_EVENTS, NULL);
        int height = 0;
        int width = 0;

        getmaxyx(stdscr, height, width);
        _windowSize = {static_cast<size_t>(width), static_cast<size_t>(height)};
        _event = MEVENT{};
    }

    void Ncurses::ncursesRect::setBorder(Texture texture)
    {
        _borderColor_id = textureToColor(texture);
        _borderPair_id = pairFor(_borderColor_id, _hasBkdg ? _color_id : -1);
        _hasBorder = true;
    }

    void Ncurses::stop() {
        attrset(A_NORMAL);
        bkgd(' ' | COLOR_PAIR(0));
        erase();
        refresh();
        endwin();
    }

    void Ncurses::setMusic([[maybe_unused]]std::string filepath) {
        return;
    }

    void Ncurses::playSound([[maybe_unused]]std::string filepath) const {
        return;
    }

    void Ncurses::ncursesRect::display() const {
        if (_hasBkdg) {
            wbkgd(_win, COLOR_PAIR(_pair_id));
            werase(_win);
        }
        if (_hasBorder) {
            wattron(_win, COLOR_PAIR(_borderPair_id));
            box(_win, 0, 0);
            wattroff(_win, COLOR_PAIR(_borderPair_id));
        }
        if (_text != "") {
            int textPair = (_textPair_id != 0) ? _textPair_id : _pair_id;
            wattron(_win, COLOR_PAIR(textPair));
            mvwprintw(_win, 1, 1, "%s", _text.c_str());
            wattroff(_win, COLOR_PAIR(textPair));
        }
        wnoutrefresh(_win);
    }

    std::shared_ptr<IRect> Ncurses::createRect(Bounds bound) {
        WINDOW *win = newwin(bound.h, bound.w, bound.y, bound.x);
        if (win == nullptr)
            throw Exception("Failed to create ncurses window");
        auto rect = std::make_shared<ncursesRect>(_colors_id, _pairs_id, _event);
        box(win, 0, 0);
        rect->setWin(win);
        return rect;
    }

    void Ncurses::ncursesRect::setSize(Size size) {
        wresize(_win, size.h, size.w);
    }

    Bounds Ncurses::ncursesRect::getBounds() const {
        int height, width;
        getmaxyx(_win, height, width);
        int y, x;
        getbegyx(_win, y, x);
        return {static_cast<std::size_t>(width), static_cast<std::size_t>(height), static_cast<std::size_t>(x), static_cast<std::size_t>(y)};
    }

    bool Ncurses::ncursesRect::isMouseOver() const
    {
        int win_y, win_x;
        int win_h, win_w;
        getbegyx(_win, win_y, win_x);
        getmaxyx(_win, win_h, win_w);
        if (_event.y >= win_y && _event.y < win_y + win_h &&
            _event.x >= win_x && _event.x < win_x + win_w) {
            return true;
        }

        return false;
    }

    void Ncurses::ncursesRect::setPosition(Position pos)
    {
        mvwin(_win, pos.y, pos.x);
    }

    void Ncurses::setBackground(Texture texture)
    {
        _bkdg_color = textureToColor(texture);
        _bkdg_pair = pairFor(COLOR_WHITE, _bkdg_color);
        bkgd(' ' | COLOR_PAIR(_bkdg_pair));
        erase();
    }

    void Ncurses::ncursesRect::setText(std::string text, Texture texture)
    {
        _textColor_id = textureToColor(texture);
        _textPair_id = pairFor(_textColor_id, _hasBkdg ? _color_id : -1);
        _text = text;
    }

    void Ncurses::ncursesRect::setTexture(Texture texture)
    {
        _color_id = textureToColor(texture);
        _pair_id = pairFor(COLOR_BLACK, _color_id);
        if (_text != "")
            _textPair_id = pairFor(_textColor_id, _color_id);
        _hasBkdg = true;
    }

    void Ncurses::clearWindow() const
    {
        erase();
        wnoutrefresh(stdscr);
    }

    void Ncurses::render()
    {
        doupdate();
    }

    EEvent Ncurses::pollEvent()
    {
        int input = getch();
        switch (input) {
            case KEY_UP:
                return UP;
                break;
            case KEY_MOUSE:
                if (getmouse(&_event) == OK && _event.bstate & BUTTON1_CLICKED)
                    return CLICK;
                break;
            case KEY_LEFT:
                return LEFT;
                break;
            case KEY_RIGHT:
                return RIGHT;
                break;
            case KEY_DOWN:
                return DOWN;
                break;
            case KEY_ENTER:
            case '\n':
                return ENTER;
                break;
            case 27:
                return ESCAPE;
                break;
            default:
                break;
        }
        return UNDEFINED;
    }
}
