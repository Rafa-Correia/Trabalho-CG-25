#include "camera.hpp"

//constructor

camera::camera(float pos_x, float pos_y, float pos_z, float lookat_x, float lookat_y, float lookat_z, float up_x, float up_y, float up_z, std::vector<float> l_pos) {
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

    cartesian_to_spherical_coords();

    pitch = radian_to_degree(asin(dir_y));
    yaw = -radian_to_degree(atan2(dir_x, dir_z)) + 90.0f;

    if(l_pos.size() == 0) {
        lock_points.push_back(0.0f);
        lock_points.push_back(0.0f);
        lock_points.push_back(0.0f);
    }

    lock_points.insert(lock_points.end(), l_pos.begin(), l_pos.end());
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
            cartesian_to_spherical_coords();
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
            spherical_to_cartesian_coords();
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
    current_target = 0;

    int index = current_target * 3;

    lock_point_x = lock_points.at(index);
    lock_point_y = lock_points.at(index + 1);
    lock_point_z = lock_points.at(index + 2);

    set_animation(C_ANIMATION_CHANGE_TARGET);
}

void camera::cycle_target() {
    current_target = (current_target + 1) % (lock_points.size() / 3);

    int index = current_target * 3;

    lock_point_x = lock_points.at(index);
    lock_point_y = lock_points.at(index + 1);
    lock_point_z = lock_points.at(index + 2);

    set_animation(C_ANIMATION_CHANGE_TARGET);
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

        spherical_to_cartesian_coords();

        pitch = radian_to_degree(asin(dir_y));
        yaw = -radian_to_degree(atan2(dir_x, dir_z)) + 90.0f;
        
        just_warped = true;
        glutWarpPointer(center_x, center_y);
    }
    else {  //switched INTO locked cam
		glutSetCursor(GLUT_CURSOR_INHERIT);
        cartesian_to_spherical_coords();
        set_animation(C_ANIMATION_CAMERA_LOCKING);
    }

    glutPostRedisplay();
}

void camera::play_animations(int delta_time_ms) {
    if(current_animation == C_ANIMATION_IDLE) {
        return;
    }

    if(current_animation == C_ANIMATION_CAMERA_LOCKING) {
        animate_camera_locking(delta_time_ms);
        return;
    }

    if(current_animation == C_ANIMATION_CHANGE_TARGET) {
        animate_changing_target(delta_time_ms);
        return;
    }
}

void camera::set_animation(int animation) {
    animation_locked = true;
    animation_timer = 0;
    
    if(animation == C_ANIMATION_IDLE) {
        current_animation = C_ANIMATION_IDLE;
        animation_locked = false;
        return;
    }

    if(animation == C_ANIMATION_CAMERA_LOCKING) {
        start_target_x = pos_x + dir_x;
        start_target_y = pos_y + dir_y;
        start_target_z = pos_z + dir_z;

        is_free_camera = false;
        current_animation = C_ANIMATION_CAMERA_LOCKING;
        return;
    }

    if(animation == C_ANIMATION_CHANGE_TARGET) {
        start_target_x = pos_x + dir_x;
        start_target_y = pos_y + dir_y;
        start_target_z = pos_z + dir_z;

        start_pos_x = pos_x;
        start_pos_y = pos_y;
        start_pos_z = pos_z;

        //alpha = (float)rand() / RAND_MAX * (2*M_PI);
        alpha = 0;
        beta = 1/4.0f * M_PI;
        radius = 25.0f;

        is_free_camera = false;
        spherical_to_cartesian_coords(true);

        current_animation = C_ANIMATION_CHANGE_TARGET;   
        return;
    }
}

void camera::animate_camera_locking(int delta_time_ms) {
    animation_timer += delta_time_ms;
    //std::cout << "Current animation timer: " << animation_timer << " (delta: " << delta_time_ms << ")";

    if(animation_timer >= C_ANIMATION_CAMERA_LOCKING_DURATION) {
        set_animation(C_ANIMATION_IDLE);

        target_point_x = lock_point_x;
        target_point_y = lock_point_y;
        target_point_z = lock_point_z;

        return;
    }

    float time_alpha = (float)animation_timer / (float)C_ANIMATION_CAMERA_LOCKING_DURATION;

    //std::cout << " | Progress: " << time_alpha * 100 << "%" << std::endl;
    target_point_x = start_target_x * (1 - time_alpha) + lock_point_x * time_alpha;
    target_point_y = start_target_y * (1 - time_alpha) + lock_point_y * time_alpha;
    target_point_z = start_target_z * (1 - time_alpha) + lock_point_z * time_alpha;

    glutPostRedisplay();
}

void camera::animate_changing_target(int delta_time_ms) {
    animation_timer += delta_time_ms;

    if(animation_timer >= C_ANIMATION_CHANGE_TARGET_DURATION) {
        set_animation(C_ANIMATION_IDLE);

        target_point_x = lock_point_x;
        target_point_y = lock_point_y;
        target_point_z = lock_point_z;

        spherical_to_cartesian_coords();

        return;
    }

    float time_alpha_mov = (float)animation_timer / (float)C_ANIMATION_CHANGE_TARGET_DURATION;
    float time_alpha_cam = ((float)animation_timer * 2) / ((float)C_ANIMATION_CHANGE_TARGET_DURATION);

    if(time_alpha_cam < 1) {
        target_point_x = start_target_x * (1 - time_alpha_cam) + lock_point_x * time_alpha_cam;
        target_point_y = start_target_y * (1 - time_alpha_cam) + lock_point_y * time_alpha_cam;
        target_point_z = start_target_z * (1 - time_alpha_cam) + lock_point_z * time_alpha_cam;
    }

    

    pos_x = start_pos_x * (1 - time_alpha_mov) + target_pos_x * time_alpha_mov;
    pos_y = start_pos_y * (1 - time_alpha_mov) + target_pos_y * time_alpha_mov;
    pos_z = start_pos_z * (1 - time_alpha_mov) + target_pos_z * time_alpha_mov;

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

void camera::cartesian_to_spherical_coords() {
    //std::cout << "CTS" << std::endl;
    float rel_pos_x, rel_pos_y, rel_pos_z;

    rel_pos_x = pos_x - lock_point_x;
    rel_pos_y = pos_y - lock_point_y;
    rel_pos_z = pos_z - lock_point_z;

    radius = sqrt(rel_pos_x * rel_pos_x + rel_pos_y * rel_pos_y + rel_pos_z * rel_pos_z);
    beta = asin(rel_pos_y / radius);
    alpha = atan2(rel_pos_x, rel_pos_z);
}

void camera::spherical_to_cartesian_coords(bool to_target) {

    float rel_pos_x, rel_pos_y, rel_pos_z;

    rel_pos_x = radius * cos(beta) * sin(alpha);
	rel_pos_y = radius * sin(beta);
	rel_pos_z = radius * cos(beta) * cos(alpha);

    if(to_target) {
        target_pos_x = rel_pos_x + lock_point_x;
        target_pos_y = rel_pos_y + lock_point_y;
        target_pos_z = rel_pos_z + lock_point_z;

        return;
    }
    
    pos_x = rel_pos_x + lock_point_x;
    pos_y = rel_pos_y + lock_point_y;
    pos_z = rel_pos_z + lock_point_z;

    dir_x = lock_point_x - pos_x;
    dir_y = lock_point_y - pos_y;
    dir_z = lock_point_z - pos_z;

    normalize_dir();
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

float camera::radian_to_degree(float ang_r) {
    return ang_r * (180.0f / M_PI);
}
