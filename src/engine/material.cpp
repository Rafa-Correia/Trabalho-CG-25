#include "engine/material.hpp"

material::material()
{
    this->diffuse = vector3(200, 200, 200) / 255;
    this->ambient = vector3(50, 50, 50) / 255;
    this->specular = vector3(0, 0, 0) / 255;
    this->emmissive = vector3(0, 0, 0) / 255;
    this->shininess = 0;
}

material::material(vector3 diffuse_component, vector3 ambient_component, vector3 specular_component, vector3 emmissive_component, float shininess)
{
    this->diffuse = diffuse_component / 255;
    this->ambient = ambient_component / 255;
    this->specular = specular_component / 255;
    this->emmissive = emmissive_component / 255;
    this->shininess = shininess;
}

void material::apply_material()
{
    GLfloat diffuse[] = {this->diffuse.x, this->diffuse.y, this->diffuse.z, 1.0f};
    GLfloat ambient[] = {this->ambient.x, this->ambient.y, this->ambient.z, 1.0f};
    GLfloat specular[] = {this->specular.x, this->specular.y, this->specular.z, 1.0f};
    GLfloat emmissive[] = {this->emmissive.x, this->emmissive.y, this->emmissive.z, 1.0f};

    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emmissive);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
}