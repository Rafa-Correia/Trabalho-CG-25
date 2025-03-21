#include "camera.hpp"

//constructor

camera::camera(float pos_x, float pos_y, float pos_z, float lookat_x, float lookat_y, float lookat_z, float up_x, float up_y, float up_z, std::vector<vector3> l_pos) {
    pos = vector3(pos_x, pos_y, pos_z);
    up = vector3(up_x, up_y, up_z);
    dir = vector3(lookat_x - pos_x, lookat_y - pos_y, lookat_z - pos_z);

    lock_point = vector3(lookat_x, lookat_y, lookat_z);
    target_point = lock_point;

    dir.normalize();

    cartesian_to_spherical_coords();

    pitch = radian_to_degree(asin(dir.y));
    yaw = -radian_to_degree(atan2(dir.x, dir.z)) + 90.0f;

    if(l_pos.size() == 0) {
        lock_point_list.push_back(vector3());
    }

    lock_point_list.insert(lock_point_list.end(), l_pos.begin(), l_pos.end());
}

//camera movement / orientation

void camera::update_camera_position(const bool key_states[256], int delta_time_ms) {
    if(animation_locked) return;
    float delta_time_sec = delta_time_ms / 1000.0f;
    bool key_pressed = false;

    if(is_free_camera) {
        if(key_states['w'] || key_states['W']) {
            pos += dir * linear_movement_speed * delta_time_sec;

            key_pressed = true;
        }
        if(key_states['s'] || key_states['S']) {
            pos -= dir * linear_movement_speed * delta_time_sec;

            key_pressed = true;
        }
        if(key_states['a'] || key_states['A']) {
            vector3 cross = vector3::cross(up, dir);
            cross.y = 0;
            cross.normalize();
            
            pos += cross * linear_movement_speed * delta_time_sec;

            key_pressed = true;
        }
        if(key_states['d'] || key_states['D']) {
            vector3 cross = vector3::cross(up, dir);
            cross.y = 0;
            cross.normalize();
            
            pos -= cross * linear_movement_speed * delta_time_sec;

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

    dir.x = cosf(yaw * (M_PI / 180.0f)) * cosf(pitch * (M_PI / 180.0f));
    dir.y = sinf(pitch * (M_PI / 180.0f));
    dir.z = sinf(yaw * (M_PI / 180.0f)) * cosf(pitch * (M_PI / 180.0f));

    dir.normalize();

    glutWarpPointer(center_x, center_y);
    just_warped = true;

    glutPostRedisplay();
}

void camera::reset_camera() {
    if(animation_locked == true) return;
    current_target = 0;

    int index = current_target * 3;

    lock_point = lock_point_list.at(index);

    set_animation(C_ANIMATION_CHANGE_TARGET);
}

void camera::cycle_target() {
    if(animation_locked == true) return;
    current_target = (current_target + 1) % (lock_point_list.size());

    lock_point = lock_point_list.at(current_target);

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

        pitch = radian_to_degree(asin(dir.y));
        yaw = -radian_to_degree(atan2(dir.x, dir.z)) + 90.0f;
        
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
        start_target_point = pos + dir;

        is_free_camera = false;
        current_animation = C_ANIMATION_CAMERA_LOCKING;
        return;
    }

    if(animation == C_ANIMATION_CHANGE_TARGET) {
        start_target_point = pos + dir;
        start_pos = pos;

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

    if(animation_timer >= C_ANIMATION_CAMERA_LOCKING_DURATION) {
        set_animation(C_ANIMATION_IDLE);
        target_point = lock_point;

        return;
    }

    float time_alpha = (float)animation_timer / (float)C_ANIMATION_CAMERA_LOCKING_DURATION;
    target_point = start_target_point * (1 - time_alpha) + lock_point * time_alpha;

    glutPostRedisplay();
}

void camera::animate_changing_target(int delta_time_ms) {
    animation_timer += delta_time_ms;

    if(animation_timer >= C_ANIMATION_CHANGE_TARGET_DURATION) {
        set_animation(C_ANIMATION_IDLE);

        target_point = lock_point;

        spherical_to_cartesian_coords();

        return;
    }

    float time_alpha_mov = (float)animation_timer / (float)C_ANIMATION_CHANGE_TARGET_DURATION;
    float time_alpha_cam = ((float)animation_timer * 2) / ((float)C_ANIMATION_CHANGE_TARGET_DURATION);

    if(time_alpha_cam < 1) {
        target_point = start_target_point * (1 - time_alpha_cam) + lock_point * time_alpha_cam;
    }
    pos = start_pos * (1 - time_alpha_mov) + target_pos * time_alpha_mov;

    glutPostRedisplay();
}

//render important

void camera::camera_glu_lookat() {
    glLoadIdentity();
    if(is_free_camera) {
        target_point = pos + dir;
    }
    gluLookAt(pos.x, pos.y, pos.z, target_point.x, target_point.y, target_point.z, up.x, up.y, up.z);
}

//AUXILIARY FUNCTIONS

void camera::cartesian_to_spherical_coords() {
    vector3 rel_pos;

    rel_pos = pos - lock_point;

    radius = sqrt(rel_pos.x * rel_pos.x + rel_pos.y * rel_pos.y + rel_pos.z * rel_pos.z);
    beta = asin(rel_pos.y / radius);
    alpha = atan2(rel_pos.x, rel_pos.z);
}

void camera::spherical_to_cartesian_coords(bool to_target) {
    vector3 rel_pos;

    rel_pos.x = radius * cos(beta) * sin(alpha);
	rel_pos.y = radius * sin(beta);
	rel_pos.z = radius * cos(beta) * cos(alpha);

    if(to_target) {
        target_pos = rel_pos + lock_point;

        return;
    }
    
    pos = rel_pos + lock_point;
    dir = lock_point - pos;

    dir.normalize();
}

void camera::print_info() {
    std::cout << "Position: " << pos.x << " " << pos.y << " " << pos.z << "\n";
    std::cout << "Lock point: " << lock_point.x << " " << lock_point.y << " " << lock_point.z << "\n";
    std::cout << "Up vector: " << up.x << " " << up.y << " " << up.z << "\n";
    std::cout << "Direction vector: " << dir.x << " " << dir.y << " " << dir.z << "\n";
    std::cout << "Spherical coords: " << radius << " " << alpha << " " << beta << "\n";
    std::cout << "Pitch/Yaw: " << pitch << " " << yaw << "\n-|- >< \n\n" << std::endl;
}

float camera::radian_to_degree(float ang_r) {
    return ang_r * (180.0f / M_PI);
}
