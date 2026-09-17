/*
** EPITECH PROJECT, 2025
** G-OOP-400-LIL-4-1-arcade-9
** File description:
** DLLoader.hpp
*/

#pragma once
#include <iostream>
#include <dlfcn.h>
#include <stdexcept>
#include "IDisplayModule.hpp"

template<typename T>
class DLLoader {
    public:
        DLLoader() = default;
        ~DLLoader() = default;
        void *open(std::string filepath) {
            void *handle;
            handle = dlopen(filepath.c_str(), RTLD_LAZY);
            if (!handle) {
                throw std::runtime_error(std::string("Failed to load library: ") + dlerror());
            }
            return handle;
        }
        T *getInstance(void *handle) {
            char *error = NULL;
            T *(*myEntryPoint)(void);
            dlerror();
            *(void**) (&myEntryPoint) = dlsym(handle, "myEntryPoint");
            error = dlerror();
            if (error != NULL) {
                throw std::runtime_error(std::string("Failed to find myEntryPoint symbol: ") + error);
            }
            T *obj = myEntryPoint();
            return obj;
        }

        arcade::EType getType(void *handle)
        {
            char *error = NULL;
            dlerror();
            arcade::EType (*getTypeLib)(void);
            *(void**)(&getTypeLib) = dlsym(handle, "getLibType");
            error = dlerror();
            if (error != NULL) {
                throw std::runtime_error(std::string("Failed to find getLibType symbol: ") + error);
            }
            arcade::EType type = getTypeLib();
            return type;
        }

        std::string getName(void *handle)
        {
            char *error = NULL;
            dlerror();
            std::string (*getNameLib)(void);
            *(void**)(&getNameLib) = dlsym(handle, "getLibName");
            error = dlerror();
            if (error != NULL) {
                throw std::runtime_error(std::string("Failed to find getLibName symbol: ") + error);
            }
            std::string name = getNameLib();
            return name;
        }
        void close(void *handle) {dlclose(handle);}
};