#ifndef TRANSLATION_HPP
#define TRANSLATION_HPP

#include "vector3.hpp"
#include "vector4.hpp"
#include "matrix4x4.hpp"

#include <vector>
#include <iostream>
#include <tuple>

#define _USE_MATH_DEFINES
#include <math.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/glut.h>
#endif

#define PATH_DIVISIONS 100

class translation
{
public:
    virtual void update(int delta_time_ms) = 0;
    virtual matrix4x4 get_translation() = 0;
    virtual void draw_path() = 0;

    virtual operator const matrix4x4() const = 0;
};

#endif