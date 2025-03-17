#ifndef CAMERA_HPP
#define CAMERA_HPP

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

        bool update_camera_position(const bool key_states[256], int delta_time_ms);
        void update_camera_direction(int x, int y);
        void update_window_size(int width, int height);
        void camera_glu_lookat();
    private:
        float pos_x = 0, pos_y = 0, pos_z = 0;
        float up_x = 0, up_y = 1, up_z = 0;

        float dir_x = 0.0f;
        float dir_y = 0.0f;
        float dir_z = 0.0f;

        float movement_speed = 5.0f; //2 units per second

        float mouse_sensitivity = 0.06f;

        float yaw = 0.0f;
        float pitch = 0.0f;

        int center_x = 0;
        int center_y = 0;

        void normalize_dir();

};

#endif