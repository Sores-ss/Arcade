/*
** EPITECH PROJECT, 2025
** G-OOP-400-LIL-4-1-arcade-9
** File description:
** DLLoader.hpp
*/

#pragma once
#include <iostream>
#include <dlfcn.h>

template<typename T>
class DLLoader {
    public :
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
        void close(void *handle) {dlclose(handle);}
};