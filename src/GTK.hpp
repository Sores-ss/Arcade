/*
** EPITECH PROJECT, 2025
** G-OOP-400-LIL-4-1-arcade-9
** File description:
** GTK.hpp
*/

#pragma once

#include "IDisplayModule.hpp"
#include <gtk/gtk.h>
#include <unordered_map>
#include <string>

namespace arcade {
    struct CssCache {
        std::unordered_map<std::string, std::string> map;
        std::size_t counter = 0;
        void clear() { map.clear(); }
        const std::string &get(const std::string &key,
            const Texture &bg, const Texture &fg,
            const Texture &border, bool hasBorder);
    };
    class GTK : public IDisplayModule {
        protected:
            GtkWidget *_window = nullptr;
            GtkWidget *_fixed = nullptr;
            Size _windowSize   = {0, 0};
            EEvent _pendingEvent = EEvent::UNDEFINED;
            bool _running = false;
            Texture _background = {"", 20, 20, 20, 255};
            CssCache _cssCache;
        public:
            class GTKRect : public IRect {
                private:
                    GtkWidget *_widget = nullptr;
                    GtkWidget *_parent = nullptr;
                    CssCache *_cache = nullptr;
                    Bounds _bounds = {0, 0, 0, 0};
                    std::string _text = "";
                    Texture _texture = {"", 180, 180, 180, 255};
                    Texture _border = {"", 255, 255, 255, 255};
                    Texture _textColor = {"", 255, 255, 255, 255};
                    bool _hasBorder = false;
                    std::string _appliedClass = "";
                    bool _stylesDirty  = true;
                    void applyStyleIfNeeded();
                public:
                    GTKRect() = default;
                    ~GTKRect();
                    void attachTo(GtkWidget *parent, CssCache *cache);
                    void invalidate();
                    Bounds getBounds() const override { return _bounds;}
                    bool isMouseOver() const override;
                    void setSize(Size size) override;
                    void setPosition(Position position) override;
                    void setText(std::string text, Texture tex)  override;
                    void setBorder(Texture texture) override;
                    void setTexture(Texture texture) override;
                    void display() const override;
                    GtkWidget *getWidget() const { return _widget; }
            };
            GTK() = default;
            ~GTK() = default;
            static std::string rgbaCss(const arcade::Texture &t);
            static std::string makeCssKey(const arcade::Texture &bg, const arcade::Texture &fg, 
                const arcade::Texture &border, bool hasBorder);
            static void drainMainContext();
            static void on_rect_widget_destroy([[maybe_unused]] GtkWidget *w, gpointer user_data);
            static void on_window_destroy([[maybe_unused]] GtkWidget *w, gpointer user_data);
            static gboolean on_key_pressed([[maybe_unused]] GtkWidget *widget,
                GdkEventKey *event,
                gpointer user_data);
            static gboolean on_click_pressed([[maybe_unused]] GtkWidget *widget,
                [[maybe_unused]] GdkEventButton *event,
                gpointer user_data);
            void init(std::string name, Size size) override;
            const Size getWindowSize() const override;
            void stop() override;
            void setBackground(Texture texture) override;
            std::shared_ptr<IRect> createRect(Bounds bounds) override;
            void setMusic([[maybe_unused]]std::string filepath) override {};
            void playSound([[maybe_unused]]std::string filepath) const override {};
            void render() override;
            EEvent pollEvent() override;
            void clearWindow() const  override;
            void queueEvent(EEvent event);
            void markStopped();
    };

}
