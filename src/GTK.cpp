/*
** EPITECH PROJECT, 2025
** G-OOP-400-LIL-4-1-arcade-9
** File description:
** GTK.cpp
*/

#include "GTK.hpp"
#include <sstream>

__attribute__((constructor)) void create()
{
    return;
}

__attribute__((destructor)) void destroy()
{
    return;
}

extern "C" {
    arcade::GTK *myEntryPoint()
    {
        return new arcade::GTK();
    }
    
    arcade::EType getLibType()
    {
        return arcade::EType::GRAPHICAL;
    }

    const std::string getLibName()
    {
        return "GTK";
    }
}

namespace arcade {
    std::string GTK::rgbaCss(const arcade::Texture &t)
    {
        std::ostringstream s;
        s << "rgba(" << static_cast<int>(t.r)
            << ',' << static_cast<int>(t.g)
            << ',' << static_cast<int>(t.b)
            << ',' << (static_cast<float>(t.a) / 255.0f) << ')';
        return s.str();
    }

    std::string GTK::makeCssKey(const arcade::Texture &bg, const arcade::Texture &fg,
        const arcade::Texture &border, bool hasBorder)
    {
        std::ostringstream k;
        k << (int)bg.r<<','<<(int)bg.g<<','<<(int)bg.b<<','<<(int)bg.a<<'|'
            << (int)fg.r<<','<<(int)fg.g<<','<<(int)fg.b<<','<<(int)fg.a<<'|';
        if (hasBorder)
            k<<(int)border.r<<','<<(int)border.g<<','<<(int)border.b<<','<<(int)border.a;
        else
            k << "none";
        return k.str();
    }

    void GTK::drainMainContext()
    {
        while (g_main_context_pending(nullptr))
            g_main_context_iteration(nullptr, FALSE);
    }


    void GTK::on_rect_widget_destroy([[maybe_unused]] GtkWidget *w, gpointer user_data)
    {
        auto *rect = static_cast<arcade::GTK::GTKRect *>(user_data);
        if (rect)
            rect->invalidate();
    }

    void GTK::on_window_destroy([[maybe_unused]] GtkWidget *w, gpointer user_data)
    {
        auto *gtk = static_cast<arcade::GTK *>(user_data);
        if (gtk)
            gtk->markStopped();
    }

    gboolean GTK::on_key_pressed([[maybe_unused]] GtkWidget *widget,
        GdkEventKey *event,
        gpointer user_data)
    {
        (void)widget;
        auto *gtk = static_cast<arcade::GTK *>(user_data);
        if (!gtk)
            return FALSE;
        bool handled = true;
        switch (event->keyval) {
            case GDK_KEY_Escape:
                gtk->queueEvent(arcade::EEvent::ESCAPE);
                break;
            case GDK_KEY_Up:
                gtk->queueEvent(arcade::EEvent::UP);
                break;
            case GDK_KEY_Down:
                gtk->queueEvent(arcade::EEvent::DOWN);
                break;
            case GDK_KEY_Left:
                gtk->queueEvent(arcade::EEvent::LEFT);
                break;
            case GDK_KEY_Right:
                gtk->queueEvent(arcade::EEvent::RIGHT);
                break;
            case GDK_KEY_Return:
                gtk->queueEvent(arcade::EEvent::ENTER);
                break;
            case GDK_KEY_Tab:
                gtk->queueEvent(arcade::EEvent::TAB);
                break;
            case GDK_KEY_BackSpace:
                gtk->queueEvent(arcade::EEvent::BACKSPACE);
                break;
            default:
                if (event->keyval >= GDK_KEY_a && event->keyval <= GDK_KEY_z) {
                    gtk->queueEvent(static_cast<arcade::EEvent>(
                        arcade::EEvent::A + (event->keyval - GDK_KEY_a)));
                } else if (event->keyval >= GDK_KEY_A && event->keyval <= GDK_KEY_Z) {
                    gtk->queueEvent(static_cast<arcade::EEvent>(
                        arcade::EEvent::A + (event->keyval - GDK_KEY_A)));
                } else {
                    handled = false;
                }
                break;
        }
        return handled ? TRUE : FALSE;
    }

    gboolean GTK::on_click_pressed([[maybe_unused]] GtkWidget *widget,
        [[maybe_unused]] GdkEventButton *event,
        gpointer user_data)
    {
        (void)widget;
        auto *gtk = static_cast<arcade::GTK *>(user_data);
        if (gtk)
            gtk->queueEvent(arcade::EEvent::CLICK);
        return TRUE;
    }

    const std::string &CssCache::get(const std::string &key,
        const Texture &bg, const Texture &fg,
        const Texture &border, bool hasBorder)
    {
        auto it = map.find(key);
        if (it != map.end())
            return it->second;
        std::string cls = "arc-" + std::to_string(counter++);
        std::ostringstream css;
        css << '.' << cls << " {"
            << "background:"   << GTK::rgbaCss(bg)  << ';'
            << "color:"        << GTK::rgbaCss(fg)  << ';'
            << "border-radius:0px;"
            << "padding:0;margin:0;min-width:0;min-height:0;";
        if (hasBorder)
            css << "border:2px solid " << GTK::rgbaCss(border) << ';';
        else
            css << "border:none;";
        css << '}';
        GtkCssProvider *provider = gtk_css_provider_new();
        gtk_css_provider_load_from_data(provider, css.str().c_str(), -1, nullptr);
        gtk_style_context_add_provider_for_screen(
            gdk_screen_get_default(),
            GTK_STYLE_PROVIDER(provider),
            GTK_STYLE_PROVIDER_PRIORITY_USER);
        g_object_unref(provider);

        auto [ins, ok] = map.emplace(key, std::move(cls));
        (void)ok;
        return ins->second;
    }

    void GTK::init(std::string name, Size size)
    {
        if (_window != nullptr)
            stop();
        int argc = 0;
        char **argv = nullptr;
        gtk_init(&argc, &argv);
        _windowSize = size;
        _window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
        _fixed = gtk_fixed_new();
        GtkStyleContext *fixedCtx = gtk_widget_get_style_context(_fixed);
        gtk_style_context_add_class(fixedCtx, "arc-fixed");
        {
            GtkCssProvider *p = gtk_css_provider_new();
            gtk_css_provider_load_from_data(p, ".arc-fixed { background: transparent; padding:0; margin:0; }", -1, nullptr);
            gtk_style_context_add_provider_for_screen(gdk_screen_get_default(), GTK_STYLE_PROVIDER(p), GTK_STYLE_PROVIDER_PRIORITY_USER);
            g_object_unref(p);
        }
        gtk_container_add(GTK_CONTAINER(_window), _fixed);
        gtk_window_set_title(GTK_WINDOW(_window), name.c_str());
        gtk_window_set_default_size(GTK_WINDOW(_window),
            static_cast<int>(size.w), static_cast<int>(size.h));
        gtk_window_set_resizable(GTK_WINDOW(_window), FALSE);
        gtk_widget_add_events(_window, GDK_KEY_PRESS_MASK | GDK_BUTTON_PRESS_MASK);
        g_signal_connect(_window, "key-press-event", G_CALLBACK(on_key_pressed), this);
        g_signal_connect(_window, "button-press-event", G_CALLBACK(on_click_pressed), this);
        g_signal_connect(_window, "destroy", G_CALLBACK(on_window_destroy), this);
        gtk_widget_show_all(_window);
        _pendingEvent = EEvent::UNDEFINED;
        _running = true;
    }

    const Size GTK::getWindowSize() const { return _windowSize; }

    void GTK::stop()
    {
        _cssCache.clear();

        if (_window != nullptr && GTK_IS_WINDOW(_window)) {
            g_signal_handlers_disconnect_by_data(_window, this);
            gtk_widget_destroy(_window);
            drainMainContext();
        }
        _window = nullptr;
        _fixed = nullptr;
        _running = false;
    }

    void GTK::setBackground(Texture texture)
    {
        _background = texture;
        if (_window == nullptr || !GTK_IS_WINDOW(_window))
            return;
        GtkCssProvider *p = gtk_css_provider_new();
        std::string css = "window { background: " + rgbaCss(texture) + "; }";
        gtk_css_provider_load_from_data(p, css.c_str(), -1, nullptr);
        gtk_style_context_add_provider_for_screen(gdk_screen_get_default(), GTK_STYLE_PROVIDER(p), GTK_STYLE_PROVIDER_PRIORITY_USER);
        g_object_unref(p);
    }

    std::shared_ptr<IRect> GTK::createRect(Bounds bounds)
    {
        auto rect = std::make_shared<GTKRect>();
        rect->attachTo(_fixed, &_cssCache);
        rect->setPosition({bounds.x, bounds.y});
        rect->setSize({bounds.w, bounds.h});
        return rect;
    }

    void GTK::render()
    {
        drainMainContext();
    }

    EEvent GTK::pollEvent()
    {
        drainMainContext();
        EEvent ev = _pendingEvent;
        _pendingEvent = EEvent::UNDEFINED;
        return ev;
    }

    void GTK::clearWindow() const
    {
        if (_fixed == nullptr || !GTK_IS_FIXED(_fixed))
            return;
        GList *children = gtk_container_get_children(GTK_CONTAINER(_fixed));
        for (GList *it = children; it != nullptr; it = it->next) {
            GtkWidget *child = GTK_WIDGET(it->data);
            if (GTK_IS_WIDGET(child))
                gtk_widget_set_visible(child, FALSE);
        }
        g_list_free(children);
    }

    void GTK::queueEvent(EEvent event) { _pendingEvent = event; }

    void GTK::markStopped()
    {
        _window = nullptr;
        _fixed = nullptr;
        _pendingEvent = EEvent::QUIT;
        _running = false;
    }

    GTK::GTKRect::~GTKRect()
    {
        if (_widget != nullptr && GTK_IS_WIDGET(_widget))
            g_signal_handlers_disconnect_by_data(_widget, this);
        _parent = nullptr;
        _widget = nullptr;
    }

    void GTK::GTKRect::invalidate()
    {
        _parent = nullptr;
        _widget = nullptr;
        _cache = nullptr;
    }

    void GTK::GTKRect::attachTo(GtkWidget *parent, CssCache *cache)
    {
        if (parent == nullptr || _widget != nullptr || !GTK_IS_FIXED(parent))
            return;
        _parent = parent;
        _cache = cache;
        _widget = gtk_button_new();

        GtkStyleContext *ctx = gtk_widget_get_style_context(_widget);
        gtk_style_context_add_class(ctx, "arc-cell");
        {
            static bool cellCssDone = false;
            if (!cellCssDone) {
                GtkCssProvider *p = gtk_css_provider_new();
                gtk_css_provider_load_from_data(p, ".arc-cell, .arc-cell > * {  padding:0; margin:0; min-width:0; min-height:0;  outline:none; box-shadow:none; border-radius:0; }", -1, nullptr);
                gtk_style_context_add_provider_for_screen(gdk_screen_get_default(), GTK_STYLE_PROVIDER(p), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
                g_object_unref(p);
                cellCssDone = true;
            }
        }
        g_signal_connect(_widget, "destroy", G_CALLBACK(on_rect_widget_destroy), this);
        gtk_widget_set_can_focus(_widget, FALSE);
        gtk_fixed_put(GTK_FIXED(_parent), _widget, static_cast<gint>(_bounds.x), static_cast<gint>(_bounds.y));
        gtk_widget_set_visible(_widget, FALSE);
    }

    bool GTK::GTKRect::isMouseOver() const
    {
        if (_widget == nullptr || !GTK_IS_WIDGET(_widget))
            return false;
        return gtk_widget_has_focus(_widget) || gtk_widget_get_state_flags(_widget) & GTK_STATE_FLAG_PRELIGHT;
    }

    void GTK::GTKRect::setSize(Size size)
    {
        _bounds.w = size.w;
        _bounds.h = size.h;
        if (_widget != nullptr && GTK_IS_WIDGET(_widget))
            gtk_widget_set_size_request(_widget, static_cast<int>(_bounds.w), static_cast<int>(_bounds.h));
    }

    void GTK::GTKRect::setPosition(Position position)
    {
        _bounds.x = position.x;
        _bounds.y = position.y;
        if (_widget != nullptr && _parent != nullptr && GTK_IS_WIDGET(_widget) && GTK_IS_FIXED(_parent))
            gtk_fixed_move(GTK_FIXED(_parent), _widget, static_cast<gint>(_bounds.x), static_cast<gint>(_bounds.y));
    }

    void GTK::GTKRect::setText(std::string text, Texture texture)
    {
        _text = std::move(text);
        if (_textColor.r != texture.r || _textColor.g != texture.g ||
            _textColor.b != texture.b || _textColor.a != texture.a)
        {
            _textColor = texture;
            _stylesDirty = true;
        }
    }

    void GTK::GTKRect::setBorder(Texture texture)
    {
        if (!_hasBorder ||
            _border.r != texture.r || _border.g != texture.g ||
            _border.b != texture.b || _border.a != texture.a)
        {
            _border = texture;
            _hasBorder = true;
            _stylesDirty = true;
        }
    }

    void GTK::GTKRect::setTexture(Texture texture)
    {
        if (_texture.r != texture.r || _texture.g != texture.g ||
            _texture.b != texture.b || _texture.a != texture.a)
        {
            _texture = texture;
            _stylesDirty = true;
        }
    }

    void GTK::GTKRect::applyStyleIfNeeded()
    {
        if (!_stylesDirty || _widget == nullptr || !GTK_IS_WIDGET(_widget))
            return;
        if (_cache == nullptr)
            return;
        GtkStyleContext *ctx = gtk_widget_get_style_context(_widget);
        if (!_appliedClass.empty())
            gtk_style_context_remove_class(ctx, _appliedClass.c_str());
        std::string key = makeCssKey(_texture, _textColor, _border, _hasBorder);
        const std::string &cls = _cache->get(key, _texture, _textColor, _border, _hasBorder);
        gtk_style_context_add_class(ctx, cls.c_str());
        _appliedClass = cls;
        _stylesDirty = false;
    }

    void GTK::GTKRect::display() const
    {
        if (_widget == nullptr || !GTK_IS_BUTTON(_widget))
            return;

        gtk_button_set_label(GTK_BUTTON(_widget), _text.c_str());
        const_cast<GTKRect *>(this)->applyStyleIfNeeded();
        gtk_widget_set_visible(_widget, TRUE);
    }
}
