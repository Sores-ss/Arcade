/*
** EPITECH PROJECT, 2025
** Visual Studio Live Share (Workspace)
** File description:
** Execption.hpp
*/

#pragma once

#include <iostream>

class Exception : public std::exception {
    public:
        explicit Exception(const std::string& message) : _message(message) {};
        ~Exception() override {};
        const char* what() const noexcept override {
            return _message.c_str();
        }
    protected:
        std::string _message;
};