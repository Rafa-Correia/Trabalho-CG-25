#ifndef LIGHT_HPP
#define LIGHT_HPP

#include <sstream>
#include <string>

#include "math/vector3.hpp"

#include "utils/printer.hpp"

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/glut.h>
#endif

class light
{
public:
    light(GLenum light, unsigned char type, vector3 position, vector3 direction, float cutoff);
    void apply_light();

private:
    GLenum _light;
    unsigned char type;
    vector3 position;
    vector3 direction;
    float cutoff;
};

#endif