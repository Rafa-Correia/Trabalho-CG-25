#ifndef MODEL_HPP
#define MODEL_HPP

#define USE_LIGHTING
// #define IGNORE_FRUSTUM_CULL

#include <iostream>
#include <sstream>
#include <fstream>
#include <exception>
#include <string>
#include <vector>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/glut.h>
#endif

#include "engine/material.hpp"
#include "engine/frustum.hpp"

#include "external/tinyxml2.h"

#include "math/vector3.hpp"
#include "math/vector4.hpp"
#include "math/matrix4x4.hpp"

#include "utils/printer.hpp"

class model
{
public:
    model() = delete;
    model(tinyxml2::XMLElement *root);
    // non_empty constructor not needed, right?

    void render_model(frustum &view_frustum, bool frustum_cull, vector3 &position, bool render_bounding_sphere, matrix4x4 &camera_transform);

private:
    GLuint VBO;
    GLuint EBO;
    GLuint NORMAL_BUFFER;
    size_t object_count;

    bool has_ebo = false;
    bool has_normals = false;

    material mat;

    vector4 bounding_sphere;

    void parse_model(tinyxml2::XMLElement *root);
    void parse_file(const std::string &filepath);
};

class FailedToParseModelException : std::exception
{
public:
    FailedToParseModelException(const std::string &msg) : message(msg) {};

    const char *what() const noexcept override
    {
        return message.c_str();
    }

private:
    std::string message;
};

#endif