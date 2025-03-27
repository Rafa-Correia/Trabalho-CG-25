#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>

#define _USE_MATH_DEFINES
#include <math.h>

#include "shape_generator.hpp"
#include "cone.hpp"
#include "sphere.hpp"
#include "cylinder.hpp"
#include "box.hpp"
#include "plane.hpp"

#ifdef _WIN32
#include <windows.h>
#endif

void printRedException(const std::string& message) {
#ifdef _WIN32
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    // Print "Exception" in red
    SetConsoleTextAttribute(hConsole, FOREGROUND_RED);
    std::cout << "Exception: ";

    // Reset color to default
    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    std::cout << " " << message << std::endl;
#else
    std::cout << "\033[31mException: \033[0m " << message << std::endl;
#endif
}

int main(int argc, char **argv)
{
    shape_generator *generator;

    std::string model_type(argv[1]);
    if (model_type.compare("sphere") == 0)
    {
        generator = new sphere_generator();
    }
    else if (model_type.compare("box") == 0)
    {
        generator = new box_generator();
    }
    else if (model_type.compare("cone") == 0)
    {
        generator = new cone_generator();
    }
    else if (model_type.compare("plane") == 0)
    {
        generator = new plane_generator();
    }
    else if (model_type.compare("cylinder") == 0) 
    {
        generator = new cylinder_generator();
    }
    else
    {
        printRedException("Invalid shape!");
        return 1;
    }

    try {
        generator->generate(argc, argv);
    }
    catch (const InvalidArgumentsException& exc) {
        printRedException(exc.what());
        return 1;
    }

    return 0;
}