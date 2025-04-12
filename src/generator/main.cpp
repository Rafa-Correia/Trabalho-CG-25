#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>

#define _USE_MATH_DEFINES
#include <math.h>

#include "generator/shape_generator.hpp"
#include "generator/cone.hpp"
#include "generator/sphere.hpp"
#include "generator/cylinder.hpp"
#include "generator/box.hpp"
#include "generator/plane.hpp"
#include "generator/torus.hpp"
#include "generator/patch.hpp"

#include "utils/printer.hpp"

#ifdef _WIN32
#include <windows.h>
#endif

int main(int argc, char **argv)
{
    // enable ansi code support on windows
#ifdef _WIN32
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
#endif

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
    else if (model_type.compare("torus") == 0)
    {
        generator = new torus_generator();
    }
    else if (model_type.compare("patch") == 0)
    {
        generator = new patch_generator();
    }
    else
    {
        printer::print_exception("Invalid shape!");
        return 1;
    }

    try
    {
        generator->generate(argc, argv);
    }
    catch (const InvalidArgumentsException &exc)
    {
        std::stringstream ss;
        ss << "gen " << model_type;
        printer::print_exception(exc.what(), ss.str());
        return 1;
    }

    return 0;
}