#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "tinyxml2.h"
#include "group.hpp"
#include "camera.hpp"
#include "matrix4x4.hpp"
#include "vector3.hpp"
#include "vector4.hpp"
#include "frustum.hpp"

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
        vector3 get_projection_settings();

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
         * 
         * @param camera_transform Projection and view transform to be able to properly render bounding sphere.
         * @param view_frustum View frustum to be passed to group rendering. Used in frustum rendering.
         * @param frustum_cull Determines if frustum culling is enabled.
         * @param render_bounding_spheres Determines if bounding spheres are to be rendered.
         */
        void render_all_groups(matrix4x4& camera_transform, frustum& view_frustum, bool frustum_cull = true, bool render_bounding_spheres = false);

        /**
         * Calls print_group() for all root groups.
         */
        void print_group_info();

    private:
        int w_width, w_height;                          // < -- Window attributes

        vector3 c_pos;                                  // < -- Initial camera position
        vector3 c_lookat;                               // < -- Initial camera lock point
        vector3 c_up;                                   // < -- Initial camera "up" vector

        //float c_fov, c_near_plane, c_far_plane;         
        vector3 projection_attributes;                  // < -- Projection attributes

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
        std::vector<vector3> query_group_postitions();

        /**
         * Function responsible for updating all group positions.
         * 
         * It isn't really needed right now, just need to call it once to make sure groups have correct positions.
         */
        void update_group_positions();
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