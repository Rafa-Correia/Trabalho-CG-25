#include "camera.hpp"

camera::camera(float pos_x, float pos_y, float pos_z, float lookat_x, float lookat_y, float lookat_z, float up_x, float up_y, float up_z) {
    this->pos_x = pos_x;
    this->pos_y = pos_y;
    this->pos_z = pos_z;
    this->up_x = up_x;
    this->up_y = up_y;
    this->up_z = up_z;

    dir_x = - pos_x + lookat_x;
    dir_y = - pos_y + lookat_y;
    dir_z = - pos_z + lookat_z;

    normalize_dir();
}

bool camera::update_camera_position(const bool key_states[256], int delta_time_ms) {
    float delta_time_sec = delta_time_ms / 1000.0f;
    
    if(key_states['w'] || key_states['W']) {
        pos_x -= dir_x * movement_speed * delta_time_sec;
        pos_y -= dir_y * movement_speed * delta_time_sec;
        pos_z -= dir_z * movement_speed * delta_time_sec;
    }
    if(key_states['s'] || key_states['S']) {
        pos_x += dir_x * movement_speed * delta_time_sec;
        pos_y += dir_y * movement_speed * delta_time_sec;
        pos_z += dir_z * movement_speed * delta_time_sec;
    }
    if(key_states['a'] || key_states['A']) {
        float cross_x = up_y * dir_z - up_z * dir_y;
        float cross_z = up_x * dir_y - up_y * dir_x;

        float cross_length = sqrt(cross_x * cross_x + cross_z * cross_z);
        
        cross_x /= cross_length;
        cross_z /= cross_length;

        pos_x -= cross_x * movement_speed * delta_time_sec;
        pos_z -= cross_z * movement_speed * delta_time_sec;
    }
    if(key_states['d'] || key_states['D']) {
        float cross_x = up_y * dir_z - up_z * dir_y;
        float cross_z = up_x * dir_y - up_y * dir_x;

        float cross_length = sqrt(cross_x * cross_x + cross_z * cross_z);

        cross_x /= cross_length;
        cross_z /= cross_length;

        pos_x += cross_x * movement_speed * delta_time_sec;
        pos_z += cross_z * movement_speed * delta_time_sec;
    }

    return true;
}

void camera::update_camera_direction(int x, int y) {
    static bool just_warped = false;

    if(just_warped) {
        just_warped = false;
        return;
    }

    float x_offset = (x - center_x) * mouse_sensitivity;
    float y_offset = (center_y - y) * mouse_sensitivity;

    yaw += x_offset;
    pitch += y_offset;

    // Clamp pitch to avoid flipping
    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    dir_x = cosf(yaw * (M_PI / 180.0f)) * cosf(pitch * (M_PI / 180.0f));
    dir_y = sinf(pitch * (M_PI / 180.0f));
    dir_z = sinf(yaw * (M_PI / 180.0f)) * cosf(pitch * (M_PI / 180.0f));

    normalize_dir();

    glutWarpPointer(center_x, center_y);
    just_warped = true;
}

void camera::update_window_size(int width, int height) {
    center_x = width / 2;
    center_y = height / 2;
}

void camera::normalize_dir() {
    float dir_vector_length = sqrt(dir_x * dir_x + dir_y * dir_y + dir_z * dir_z);

    if(dir_vector_length == 0) {
        dir_x = 1.0f;
        dir_y = 0.0f;
        dir_z = 0.0f;

        return;
    }
    //normalize direction vector
    dir_x /= dir_vector_length;
    dir_y /= dir_vector_length;
    dir_z /= dir_vector_length;
}

void camera::camera_glu_lookat() {
    glLoadIdentity();
    gluLookAt(pos_x, pos_y, pos_z, dir_x + pos_x, dir_y + pos_y, dir_z + pos_z, up_x, up_y, up_z);
}
