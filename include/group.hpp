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

        /**
         * Debug function that prints group info, as well as call this function for all sub groups.
         */
        void print_group(const std::string prepend);

        /**
         * Function that renders this group, as well as call itself for all subgroups.
         */
        void render_group();
        
        /**
         * Returns camera lock positions for this group and calls itself for all subgroups.
         * 
         * @param x_base Base x position from parent group transforms.
         * @param y_base Base y position from parent group transforms.
         * @param z_base Base z position from parent group transforms.
         */
        std::vector<float> lock_positions(float x_base, float y_base, float z_base);

        private:

        unsigned int mesh_count = 0;                                                // < -- number of loaded meshes
        
        std::vector<char> transform_order;                                          // < -- "string" containing order of transformations
        
        float translate_x, translate_y, translate_z;                                // < -- translate transform parameters
        float rotate_angle, rotate_x, rotate_y, rotate_z;                           // < -- rotate transform parameters
        float scale_x, scale_y, scale_z;                                            // < -- scale transform parameters
        
        float color_r, color_g, color_b;                                            // < -- group color, doesnt apply to subgroups. PLACEHOLDER, is randomly generated
        
        std::vector<std::vector<float>> mesh_vertices_buffer;                       // < -- buffer storing vertices until VBO created
        std::vector<std::tuple<bool, std::vector<int>>> mesh_indices_buffer;        // < -- buffer storing pair of boolean and indices. boolean = false -> no indices
        
        std::vector<std::tuple<bool, std::vector<float>>> mesh_normals_buffer;      // < -- buffer storing pair of boolean and normals. boolean = false -> no normals
        std::vector<std::tuple<bool, std::vector<float>>> mesh_tex_coords_buffer;   // < -- buffer storing pair of boolean and texture coordinates. boolean = false -> no texture coordinates
        
        
        
        std::vector<group> sub_groups;                      // < -- All loaded subgroups
        
        std::vector<GLuint> group_vbos;                     // < -- VBOs of all group meshes
        std::vector<std::tuple<bool, GLuint>> group_ebos;   // < -- Pair of boolean and EBO for all group meshes. boolean = false -> no EBO
        std::vector<size_t> vertex_or_index_count;          // < -- Vertex count if no EBO, index count if EBO
        
        /**
         * Function responsible for creating and loading data into all VBOs and EBOs when applicable.
         */
        void prepare_render();

        /**
         * Function responsible for creating a group from a root "group" XMLElement.
         * 
         * @param root Group element to parse. 
         * 
         * @returns Boolean indicating if parsing was successful.
         */
        bool parse_group(tinyxml2::XMLElement *root);

        /**
         * Function responsible for loading model file (*.3d).
         * 
         * @param filepath Path to model file.
         * 
         * @returns Boolean indicating if parsing was successful. 
         */
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

#endif