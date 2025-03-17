#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "tinyxml2.h"
#include "group.hpp"
#include "camera.hpp"

#include <exception>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <tuple>
#include <vector>

class config {
    public:
        config() = delete;
        config(const char *path);

        void print_info();

        std::tuple<int, int> get_window_attributes();

        std::tuple<float, float, float> get_projection_settings();

        std::vector<group> get_root_groups();

        camera* get_config_camera_init();

        void prepare_all_groups();
        void render_all_groups();
    private:
        //attributes

        //window attributes
        int w_width, w_height;

        //camera attributes

        float c_pos_x, c_pos_y, c_pos_z; // camera position
        float c_lookat_x, c_lookat_y, c_lookat_z; // camera lookat position
        float c_up_x, c_up_y, c_up_z; // camera up vector

        //projection attributes

        float c_fov, c_near_plane, c_far_plane; //attributes of projection


        //model stuff
        std::vector<group> root_groups;

        bool load(const char *path);
};

class FailedToLoadException : public std::exception {
    public:
        FailedToLoadException(const std::string& msg) : message(msg) {};

        const char *what() const noexcept override {
            return message.c_str();
        }

    private:
        std::string message;
};

#endif