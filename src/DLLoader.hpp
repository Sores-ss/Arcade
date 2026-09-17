/*
** EPITECH PROJECT, 2025
** G-OOP-400-LIL-4-1-arcade-9
** File description:
** DLLoader.hpp
*/

#pragma once
#include <iostream>
#include <dlfcn.h>
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
                std::fprintf(stderr, "%s\n", dlerror());
                std::exit(EXIT_FAILURE);
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
                std::fprintf(stderr, "%s\n", error);
                std::exit(EXIT_FAILURE);
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
                std::fprintf(stderr, "%s\n", error);
                std::exit(EXIT_FAILURE);
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
                std::fprintf(stderr, "%s\n", error);
                std::exit(EXIT_FAILURE);
            }
            std::string name = getNameLib();
            return name;
        }
        void close(void *handle) {dlclose(handle);}
};