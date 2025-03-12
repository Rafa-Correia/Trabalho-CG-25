#ifndef GROUP_HPP
#define GROUP_HPP

#include "tinyxml2.h"

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


class group {
    public:
        group() = delete;
        group(tinyxml2::XMLElement *root);

        void print_group(const std::string prepend);

        void prepare_render();
        void render_group();

    private:
        bool ready_to_render = false;
        unsigned int mesh_count = 0;

        std::vector<char> transform_order;

        float translate_x, translate_y, translate_z; //translation transform
        float rotate_angle, rotate_x, rotate_y, rotate_z; //rotation transform
        float scale_x, scale_y, scale_z; //scaling transform

        //vectors of mesh vertex vector and index vector
        std::vector<std::vector<float>> mesh_vertices_buffer;
        std::vector<std::tuple<bool, std::vector<int>>> mesh_indices_buffer;

        //both of these are unused for now
        std::vector<std::tuple<bool, std::vector<float>>> mesh_normals_buffer;
        std::vector<std::tuple<bool, std::vector<float>>> mesh_tex_coords_buffer;



        std::vector<group> sub_groups;

        std::vector<GLuint> group_vbos;
        std::vector<std::tuple<bool, GLuint>> group_ebos;
        std::vector<size_t> vertex_or_index_count; //if has index buffer, then represents index count, else vertex count

        bool parse_group(tinyxml2::XMLElement *root);
        bool parse_model_file(const char *filepath);
};

class FailedToParseGroupException : public std::exception {
    public:
        FailedToParseGroupException(const std::string& msg) : message(msg) {};

        const char *what() const noexcept override {
            return message.c_str();
        }

    private:
        std::string message;
};

class NotReadyToRenderException : public std::exception {
    public:
        NotReadyToRenderException(const std::string& msg) : message(msg) {};

        const char *what() const noexcept override {
            return message.c_str();
        }

    private:
        std::string message;
};

#endif