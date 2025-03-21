#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "tinyxml2.h"
#include "group.hpp"
#include "camera.hpp"
#include "matrix4x4.hpp"
#include "vector3.hpp"

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

        /**
         * Debug function printing important config class information.
         */
        void print_info();

        /**
         * Getter for window width and height.
         * 
         * @returns Window width and height in a tuple.
         */
        std::tuple<int, int> get_window_attributes();

        /**
         * Getter for projection parameters. 
         * 
         * @returns Camera FOV, near plane and far plane distances.
         */
        std::tuple<float, float, float> get_projection_settings();

        /**
         * Getter for root groups. 
         * 
         * @returns Vector containing all root groups.
         */
        std::vector<group> get_root_groups();

        /**
         * Getter for camera created from config file.
         * 
         * @returns Pointer to camera created on config loading.
         */
        camera* get_config_camera_init();

        /**
         * Calls render_group() for all root groups.
         */
        void render_all_groups();

        /**
         * Calls print_group() for all root groups.
         */
        void print_group_info();

    private:
        int w_width, w_height;                          // < -- Window attributes

        float c_pos_x, c_pos_y, c_pos_z;                // < -- Initial camera position
        float c_lookat_x, c_lookat_y, c_lookat_z;       // < -- Initial camera lock point
        float c_up_x, c_up_y, c_up_z;                   // < -- Initial camera "up" vector

        float c_fov, c_near_plane, c_far_plane;         // < -- Projection attributes

        camera *cam;                                    // < -- Camera object created with initial configuration.

        std::vector<group> root_groups;                 // < -- Vector of all root groups (groups with no parent group)

        /**
         * Function responsible for loading and parsing a configuration file.
         * 
         * @param path File path to configuration file.
         * 
         * @returns Boolean indicating if loading process was a success.
         */
        bool load(const char *path);

        /**
         * Getter for all groups and their sub group positions for camera locking.
         * 
         * @returns All positions of groups and subgroups for camera locking. Each position is 3 floats!
         */
        std::vector<vector3> lock_postitions();
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