/*
** EPITECH PROJECT, 2025
** Visual Studio Live Share (Workspace)
** File description:
** SDL2.hpp
*/

#pragma once

#include "IDisplayModule.hpp"

namespace arcade {
    class SDL2 : public IDisplayModule {
        protected:
            EType _type = GRAPHICAL;
        public:
            SDL2() = default;
            ~SDL2() = default;
            void init() override;
            void stop() override;
            const std::string getName() const override;
            const EType &getType() const override;
            IButton *createButton(size_t width, size_t height, size_t x, size_t y) override;
    };
}
