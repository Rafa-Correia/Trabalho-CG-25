#ifndef SHAPE_GENERATOR_HPP
#define SHAPE_GENERATOR_HPP

#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>

#define _USE_MATH_DEFINES
#include <math.h>

class shape_generator {
    public:
        virtual void generate(int argc, char **argv) const = 0;
        virtual ~shape_generator() = default;

        static bool validate_filepath(const std::string &filepath)
        {
            const std::string extention = ".3d";
            if (filepath.length() <= extention.length())
            {
                return false;
            }
            return filepath.compare(filepath.length() - extention.length(), extention.length(), extention) == 0;
        }
};

class InvalidArgumentsException : public std::exception {
    public:
        InvalidArgumentsException(const std::string& msg) : message(msg) {};

        const char *what() const noexcept override {
            return message.c_str();
        }

    private:
        std::string message;
};

#endif