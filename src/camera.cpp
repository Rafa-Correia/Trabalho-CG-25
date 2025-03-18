#include "camera.hpp"

//constructor

camera::camera(float pos_x, float pos_y, float pos_z, float lookat_x, float lookat_y, float lookat_z, float up_x, float up_y, float up_z) {
    this->pos_x = pos_x;
    this->pos_y = pos_y;
    this->pos_z = pos_z;

    this->up_x = up_x;
    this->up_y = up_y;
    this->up_z = up_z;

    this->dir_x =  lookat_x - pos_x;
    this->dir_y =  lookat_y - pos_y;
    this->dir_z =  lookat_z - pos_z;

    this->lock_point_x = lookat_x;
    this->lock_point_y = lookat_y;
    this->lock_point_z = lookat_z;

    this->target_point_x = lookat_x;
    this->target_point_y = lookat_y;
    this->target_point_z = lookat_z;

    normalize_dir();

    cardinal_to_spherical_coords();

    pitch = radian_to_angle(asin(dir_y));
    yaw = -radian_to_angle(atan2(dir_x, dir_z)) + 90.0f;
}

//camera movement / orientation

void camera::update_camera_position(const bool key_states[256], int delta_time_ms) {
    if(animation_locked) return;
    float delta_time_sec = delta_time_ms / 1000.0f;
    bool key_pressed = false;

    if(is_free_camera) {
        if(key_states['w'] || key_states['W']) {
            pos_x += dir_x * linear_movement_speed * delta_time_sec;
            pos_y += dir_y * linear_movement_speed * delta_time_sec;
            pos_z += dir_z * linear_movement_speed * delta_time_sec;

            key_pressed = true;
        }
        if(key_states['s'] || key_states['S']) {
            pos_x -= dir_x * linear_movement_speed * delta_time_sec;
            pos_y -= dir_y * linear_movement_speed * delta_time_sec;
            pos_z -= dir_z * linear_movement_speed * delta_time_sec;

            key_pressed = true;
        }
        if(key_states['a'] || key_states['A']) {
            float cross_x = up_y * dir_z - up_z * dir_y;
            float cross_z = up_x * dir_y - up_y * dir_x;

            float cross_length = sqrt(cross_x * cross_x + cross_z * cross_z);

            cross_x /= cross_length;
            cross_z /= cross_length;

            pos_x += cross_x * linear_movement_speed * delta_time_sec;
            pos_z += cross_z * linear_movement_speed * delta_time_sec;

            key_pressed = true;
        }
        if(key_states['d'] || key_states['D']) {
            float cross_x = up_y * dir_z - up_z * dir_y;
            float cross_z = up_x * dir_y - up_y * dir_x;

            float cross_length = sqrt(cross_x * cross_x + cross_z * cross_z);

            cross_x /= cross_length;
            cross_z /= cross_length;

            pos_x -= cross_x * linear_movement_speed * delta_time_sec;
            pos_z -= cross_z * linear_movement_speed * delta_time_sec;

            key_pressed = true;
        }

        if(key_pressed)
            cardinal_to_spherical_coords();
    }
    else {
        if(key_states['w'] || key_states['W']) {
            beta += angular_movement_speed * delta_time_sec;

            if(beta >= M_PI / 2.0f - 0.01f) {
                beta = M_PI / 2.0f - 0.01f;
            }

            key_pressed = true;
        }
        if(key_states['s'] || key_states['S']) {
            beta -= angular_movement_speed * delta_time_sec;

            if(beta < -M_PI / 2.0f + 0.01f) {
                beta = -M_PI / 2.0f + 0.01f;
            }

            key_pressed = true;
        }
        if(key_states['a'] || key_states['A']) {
            alpha += angular_movement_speed * delta_time_sec;

            key_pressed = true;
        }
        if(key_states['d'] || key_states['D']) {
            alpha -= angular_movement_speed * delta_time_sec;

            key_pressed = true;
        }
        if(key_states['z'] || key_states['Z']) {
            radius -= zoom_in_speed * delta_time_sec;

            key_pressed = true;
        }   
        if(key_states['x'] || key_states['X']) {
            radius += zoom_in_speed * delta_time_sec;

            key_pressed = true;
        }
        
        if (key_pressed)
            spherical_to_cardinal_coords();
    }
}

void camera::update_camera_direction(int x, int y) {
    if(!is_free_camera) return;
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

    glutPostRedisplay();
}

void camera::reset_camera() {
    is_free_camera = false;
    pos_x = 15;
    pos_y = 5;
    pos_z = 0;

    target_point_x = 0;
    target_point_y = 0;
    target_point_z = 0;

    cardinal_to_spherical_coords();
}

//misc important

void camera::update_window_size(int width, int height) {
    center_x = width / 2;
    center_y = height / 2;
}

void camera::switch_camera_mode() {
    if(animation_locked) return;
    is_free_camera = !is_free_camera;

    if(is_free_camera) { //switched INTO free cam
        glutSetCursor(GLUT_CURSOR_NONE);

        spherical_to_cardinal_coords();

        pitch = radian_to_angle(asin(dir_y));
        yaw = -radian_to_angle(atan2(dir_x, dir_z)) + 90.0f;
        
        just_warped = true;
        glutWarpPointer(center_x, center_y);
    }
    else {  //switched INTO locked cam
		glutSetCursor(GLUT_CURSOR_INHERIT);
        cardinal_to_spherical_coords();
        animation_locked = true;
    }

    glutPostRedisplay();
}

void camera::play_animations(int delta_time_ms) {
    if(!animation_locked) {
        return;
    }

    if(animation_timer == 0) {
        start_target_x = target_point_x;
        start_target_y = target_point_y;
        start_target_z = target_point_z;
    }

    animation_timer += delta_time_ms;
    //std::cout << "Current animation timer: " << animation_timer << " (delta: " << delta_time_ms << ")";

    if(animation_timer >= animation_duration) {
        animation_locked = false;
        animation_timer = 0;

        target_point_x = lock_point_x;
        target_point_y = lock_point_y;
        target_point_z = lock_point_z;
        return;
    }

    float time_alpha = (float)animation_timer / (float)animation_duration;

    //std::cout << " | Progress: " << time_alpha * 100 << "%" << std::endl;
    lerp_transition(time_alpha);

    glutPostRedisplay();
}

//render important

void camera::camera_glu_lookat() {
    glLoadIdentity();
    if(is_free_camera) {
        target_point_x = dir_x + pos_x;
        target_point_y = dir_y + pos_y;
        target_point_z = dir_z + pos_z;
    }
    gluLookAt(pos_x, pos_y, pos_z, target_point_x, target_point_y, target_point_z, up_x, up_y, up_z);
}

//AUXILIARY FUNCTIONS

void camera::cardinal_to_spherical_coords() {
    //std::cout << "CTS" << std::endl;
    float rel_pos_x, rel_pos_y, rel_pos_z;

    rel_pos_x = pos_x - lock_point_x;
    rel_pos_y = pos_y - lock_point_y;
    rel_pos_z = pos_z - lock_point_z;

    radius = sqrt(rel_pos_x * rel_pos_x + rel_pos_y * rel_pos_y + rel_pos_z * rel_pos_z);
    beta = asin(rel_pos_y / radius);
    alpha = atan2(rel_pos_x, rel_pos_z);
}

void camera::spherical_to_cardinal_coords() {
    //std::cout << "STC" << std::endl;
    float rel_pos_x, rel_pos_y, rel_pos_z;

    rel_pos_x = radius * cos(beta) * sin(alpha);
	rel_pos_y = radius * sin(beta);
	rel_pos_z = radius * cos(beta) * cos(alpha);

    pos_x = rel_pos_x + lock_point_x;
    pos_y = rel_pos_y + lock_point_y;
    pos_z = rel_pos_z + lock_point_z;

    dir_x = lock_point_x - pos_x;
    dir_y = lock_point_y - pos_y;
    dir_z = lock_point_z - pos_z;

    normalize_dir();
}

void camera::lerp_transition(float time_alpha) {
    target_point_x = start_target_x * (1 - time_alpha) + lock_point_x * time_alpha;
    target_point_y = start_target_y * (1 - time_alpha) + lock_point_y * time_alpha;
    target_point_z = start_target_z * (1 - time_alpha) + lock_point_z * time_alpha;
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

void camera::print_info() {
    std::cout << "Position: " << pos_x << " " << pos_y << " " << pos_z << "\n";
    std::cout << "Lock point: " << lock_point_x << " " << lock_point_y << " " << lock_point_z << "\n";
    std::cout << "Up vector: " << up_x << " " << up_y << " " << up_z << "\n";
    std::cout << "Direction vector: " << dir_x << " " << dir_y << " " << dir_z << "\n";
    std::cout << "Spherical coords: " << radius << " " << alpha << " " << beta << "\n";
    std::cout << "Pitch/Yaw: " << pitch << " " << yaw << "\n-|- >< \n\n" << std::endl;
}

float camera::radian_to_angle(float ang_r) {
    return ang_r * (180.0f / M_PI);
}
