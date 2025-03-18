#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <iostream>

#define _USE_MATH_DEFINES
#include <math.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/glut.h>
#endif

class camera {
    public:
        camera();
        camera(float pos_x, float pos_y, float pos_z, float lookat_x, float lookat_y, float lookat_z, float up_x, float up_y, float up_z);

        /**
         * Changes camera position based on what keys are pressed. Moves at constant rate, independant on framerate.
         * 
         * @param key_states Array of booleans keeping track of what keys are being held down. 
         * @param delta_time_ms Time since last update in miliseconds.
         */
        void update_camera_position(const bool key_states[256], int delta_time_ms);

        /**
         * Changes direction camera is facing based on mouse movement. Won't change anything in fixed camera mode.
         * 
         * @param x x position of mouse cursor relative to upper right corner
         * @param y y position of mouse cursor relative to upper right corner
         */
        void update_camera_direction(int x, int y);

        /**
         * Updates internal state keeping track of window width and height.
         * 
         * @param width New width of window
         * @param height New height of window
         */
        void update_window_size(int width, int height);

        /**
         * Camera sets view matrix based on it's parameters.
         */
        void camera_glu_lookat();

        /**
         * Switches between fixed camera mode and free camera mode. Small easing animation will play on transition from free camera mode to fixed camera mode, taking control away for 0.5 sec.
         */
        void switch_camera_mode();

        /**
         * Debug function that prints camera's parameters.
         */
        void print_info();

        /**
         * Plays transition animation from free to fixed camera mode.
         * 
         * @param delta_time_ms Time since last update in ms
         */
        void play_animations(int delta_time_ms);

        /**
         * Resets camera to some default value.
         */
        void reset_camera();

    private:
    
        float pos_x = 0, pos_y = 0, pos_z = 0;                              // < -- position of the camera
        float up_x = 0, up_y = 1, up_z = 0;                                 // < -- camera's "up" vector (vector pointing up)
        float dir_x = 0.0f, dir_y = 0.0f, dir_z = 0.0f;                     // < -- camera direction, used in free cam mode
        
        float lock_point_x = 0, lock_point_y = 0, lock_point_z = 0;         // < -- camera's lock point (in fixed mode, after animation, target point will be equal to this)
        float target_point_x = 0, target_point_y = 0, target_point_z = 0;   // < -- camera always looks at this point in fixed mode
        float start_target_x, start_target_y, start_target_z;               // < -- when playing lerp camera transition, this serves as the start point
    
        float radius, alpha, beta;                                          
        float yaw = 0.0f;
        float pitch = 0.0f;

        int center_x = 0;                       //screen center x for cursor locking
        int center_y = 0;                       //screen center y for cursor locking

        int animation_timer = 0;                //ms
        int animation_duration = 500;           //ms
        
        float linear_movement_speed = 50.0f;     //units per second
        float angular_movement_speed = 1.0f;    //radians per second
        float zoom_in_speed = 5.0f;             //units per second
        float mouse_sensitivity = 0.06f;        //idk tbh
        
        bool is_free_camera = false;
        bool just_warped = false;
        bool animation_locked = false;

        /**
         * Internal function used to normalize the direction vector.
         */
        void normalize_dir();

        /**
         * Updates spherical coordinates based on the cartesian coordinates.
         */
        void cardinal_to_spherical_coords();

        /**
         * Updates cartesian coordinates based on the spherical coordinates.
         */
        void spherical_to_cardinal_coords();

        /**
         * Calculates angle value from radians to degrees.
         * 
         * @param ang_r Angle in radians
         * 
         * @returns Angle in degrees
         */
        float radian_to_angle(float ang_r);

        /**
         * Animation helper function. Updates target position according to time alpha, based on linear interpolation.
         */
        void lerp_transition(float time_alpha);
};

#endif