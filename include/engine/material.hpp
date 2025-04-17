#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include "math/vector3.hpp"

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/glut.h>
#endif

class material
{
public:
    vector3 diffuse;
    vector3 ambient;
    vector3 specular;
    vector3 emmissive;
    float shininess;

    material();
    material(vector3 diffuse_component, vector3 ambient_component, vector3 specular_component, vector3 emmissive_component, float shininess);

    void apply_material();
};

#endif