#ifndef SHAPE_GENERATOR_HPP
#define SHAPE_GENERATOR_HPP

#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>

#include "math/vector2.hpp"
#include "math/vector3.hpp"
#include "math/vector4.hpp"
#include "math/matrix4x4.hpp"
#include "math/math_utils.hpp"

#include "utils/printer.hpp"

#define _USE_MATH_DEFINES
#include <math.h>

class shape_generator
{
public:
    virtual void generate(int argc, char **argv) = 0;
    virtual ~shape_generator() = default;

    static bool validate_filepath(const std::string &filepath, const std::string &extention = ".3d")
    {
        if (filepath.length() <= extention.length())
        {
            return false;
        }
        return filepath.compare(filepath.length() - extention.length(), extention.length(), extention) == 0;
    }
};

class InvalidArgumentsException : public std::exception
{
public:
    InvalidArgumentsException(const std::string &msg) : message(msg) {};

    const char *what() const noexcept override
    {
        return message.c_str();
    }

private:
    std::string message;
};

#endif