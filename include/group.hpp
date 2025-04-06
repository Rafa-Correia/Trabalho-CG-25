#ifndef GROUP_HPP
#define GROUP_HPP

#include "tinyxml2.h"
#include "matrix4x4.hpp"
#include "vector3.hpp"
#include "vector4.hpp"
#include "frustum.hpp"

#include "transforms/rotation_dynamic.hpp"
#include "transforms/rotation_static.hpp"

#include "transforms/translation_dynamic.hpp"
#include "transforms/translation_static.hpp"


#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <tuple>
#include <unordered_set>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/glut.h>
#endif

class group
{
public:
    group() = delete;
    group(tinyxml2::XMLElement *root, float parent_scale = 1.0f);

    /**
     * Function that renders this group, as well as call itself for all subgroups.
     *
     * @param view_frustum View frustum to be used in frustum culling.
     * @param render_bounding_spheres Determines if bounding spheres are rendered.
     */
    void render_group(matrix4x4 &camera_transform, frustum &view_frustum, bool frustum_cull = true, bool render_bounding_spheres = false, bool draw_translation_path = false);

    /**
     * Returns camera lock positions for this group and calls itself for all subgroups.
     *
     * @returns Vector of vector3's containing all group positions.
     */
    std::vector<vector3> query_group_positions();

    /**
     * Updates all groups' positions.
     */
    void update_group(int delta_time_ms, matrix4x4 parent_transform);

private:
    unsigned int mesh_count = 0; // < -- number of loaded meshes

    matrix4x4 model_matrix; // < -- 4 by 4 matrix storing transformations

    unsigned char transform_order[3] = {0};
    
    translation *t = NULL;  //these need to be null since there is no default constructor
    rotation *r = NULL;
    matrix4x4 s;

    vector4 color; // < -- group color, doesnt apply to subgroups.

    vector3 position; // < -- group position in 3D space.

    std::vector<group> sub_groups; // < -- All loaded subgroups

    std::vector<GLuint> group_vbos;                   // < -- VBOs of all group meshes
    std::vector<std::tuple<bool, GLuint>> group_ebos; // < -- Pair of boolean and EBO for all group meshes. boolean = false -> no EBO
    std::vector<size_t> vertex_or_index_count;        // < -- Vertex count if no EBO, index count if EBO
    std::vector<vector4> mesh_bounding_spheres;

    /**
     * Function responsible for creating and loading data into all VBOs and EBOs when applicable.
     */
    void prepare_render();

    /**
     * Function responsible for creating a group from a root "group" XMLElement.
     *
     * @param root Group element to parse.
     */
    void parse_group(tinyxml2::XMLElement *root, float parent_scale = 1.0f);

    /**
     * Function responsible for loading model file (*.3d).
     *
     * @param filepath Path to model file.
     *
     * @returns Boolean indicating if parsing was successful.
     */
    bool parse_model_file(const char *filepath);
};

class FailedToParseGroupException : public std::exception
{
public:
    FailedToParseGroupException(const std::string &msg) : message(msg) {};

    const char *what() const noexcept override
    {
        return message.c_str();
    }

private:
    std::string message;
};

#endif