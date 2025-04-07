#include "camera.hpp"

// constructor

camera::camera(vector3 pos, vector3 target_lock_point, vector3 up, std::vector<vector3> l_pos)
{
    this->pos = pos;
    this->up = up;

    dir = target_lock_point - pos;

    this->target_lock_point = target_lock_point;
    this->lock_point = target_lock_point;

    dir.normalize();

    cartesian_to_spherical_coords(true);

    pitch = radian_to_degree(asin(dir.y));
    yaw = -radian_to_degree(atan2(dir.x, dir.z)) + 90.0f;

    if (l_pos.size() == 0)
    {
        lock_point_list.push_back(vector3());
    }

    lock_point_list.insert(lock_point_list.end(), l_pos.begin(), l_pos.end());
}

// camera movement / orientation

void camera::update_camera_position(const bool key_states[256], int delta_time_ms)
{
    if (animation_locked)
        return;
    float delta_time_sec = delta_time_ms / 1000.0f;
    bool key_pressed = false;

    if (is_free_camera)
    {
        if (key_states['w'] || key_states['W'])
        {
            pos += dir * linear_movement_speed * delta_time_sec;

            key_pressed = true;
        }
        if (key_states['s'] || key_states['S'])
        {
            pos -= dir * linear_movement_speed * delta_time_sec;

            key_pressed = true;
        }
        if (key_states['a'] || key_states['A'])
        {
            vector3 cross = vector3::cross(up, dir);
            cross.y = 0;
            cross.normalize();

            pos += cross * linear_movement_speed * delta_time_sec;

            key_pressed = true;
        }
        if (key_states['d'] || key_states['D'])
        {
            vector3 cross = vector3::cross(up, dir);
            cross.y = 0;
            cross.normalize();

            pos -= cross * linear_movement_speed * delta_time_sec;

            key_pressed = true;
        }

        if (key_pressed)
            cartesian_to_spherical_coords();
    }
    else
    {
        if (key_states['w'] || key_states['W'])
        {
            beta += angular_movement_speed * delta_time_sec;

            if (beta >= M_PI / 2.0f - 0.01f)
            {
                beta = M_PI / 2.0f - 0.01f;
            }

            key_pressed = true;
        }
        if (key_states['s'] || key_states['S'])
        {
            beta -= angular_movement_speed * delta_time_sec;

            if (beta < -M_PI / 2.0f + 0.01f)
            {
                beta = -M_PI / 2.0f + 0.01f;
            }

            key_pressed = true;
        }
        if (key_states['a'] || key_states['A'])
        {
            alpha += angular_movement_speed * delta_time_sec;

            key_pressed = true;
        }
        if (key_states['d'] || key_states['D'])
        {
            alpha -= angular_movement_speed * delta_time_sec;

            key_pressed = true;
        }
        if (key_states['z'] || key_states['Z'])
        {
            radius -= zoom_in_speed * delta_time_sec;

            key_pressed = true;
        }
        if (key_states['x'] || key_states['X'])
        {
            radius += zoom_in_speed * delta_time_sec;

            key_pressed = true;
        }

        if (key_pressed)
            spherical_to_cartesian_coords();
    }
}

void camera::update_camera_direction(int x, int y)
{
    if (!is_free_camera)
        return;
    if (just_warped)
    {
        just_warped = false;
        return;
    }

    float x_offset = (x - center_x) * mouse_sensitivity;
    float y_offset = (center_y - y) * mouse_sensitivity;
    yaw += x_offset;
    pitch += y_offset;

    // dont want the camera to turn upside down :)
    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    dir.x = cosf(yaw * (M_PI / 180.0f)) * cosf(pitch * (M_PI / 180.0f));
    dir.y = sinf(pitch * (M_PI / 180.0f));
    dir.z = sinf(yaw * (M_PI / 180.0f)) * cosf(pitch * (M_PI / 180.0f));

    dir.normalize();

    glutWarpPointer(center_x, center_y);
    just_warped = true;

    glutPostRedisplay();
}

void camera::reset_camera()
{
    if (animation_locked == true)
        return;
    current_target = 0;

    int index = current_target * 3;

    target_lock_point = lock_point_list.at(index);

    set_animation(C_ANIMATION_CHANGE_TARGET);
}

void camera::cycle_target()
{
    if (animation_locked == true)
        return;
    current_target = (current_target + 1) % (lock_point_list.size());

    target_lock_point = lock_point_list.at(current_target);

    set_animation(C_ANIMATION_CHANGE_TARGET);
}

void camera::add_to_target_radius(float delta)
{
    if (is_free_camera)
    {
        // can't change locked camera state when in free camera
        return;
    }

    target_radius += delta;

    set_animation(C_ANIMATION_CHANGE_RADIUS);
}

// misc important

void camera::update_lock_positions(std::vector<vector3> &l_pos)
{
    this->lock_point_list = l_pos;
}

void camera::update_window_size(int width, int height)
{
    center_x = width / 2;
    center_y = height / 2;
}

void camera::switch_camera_mode()
{
    if (animation_locked)
        return;
    is_free_camera = !is_free_camera;

    if (is_free_camera)
    { // switched INTO free cam
        glutSetCursor(GLUT_CURSOR_NONE);

        spherical_to_cartesian_coords();

        pitch = radian_to_degree(asin(dir.y));
        yaw = -radian_to_degree(atan2(dir.x, dir.z)) + 90.0f;

        just_warped = true;
        glutWarpPointer(center_x, center_y);
    }
    else
    { // switched INTO locked cam
        glutSetCursor(GLUT_CURSOR_INHERIT);
        cartesian_to_spherical_coords();
        set_animation(C_ANIMATION_CAMERA_LOCKING);
    }

    glutPostRedisplay();
}

// animation important

void camera::play_animations(int delta_time_ms)
{
    if (current_animation == C_ANIMATION_IDLE)
    {
        return;
    }

    if (current_animation == C_ANIMATION_CAMERA_LOCKING)
    {
        animate_camera_locking(delta_time_ms);
        return;
    }

    if (current_animation == C_ANIMATION_CHANGE_TARGET)
    {
        animate_changing_target(delta_time_ms);
        return;
    }

    if (current_animation == C_ANIMATION_CHANGE_RADIUS)
    {
        animate_radius_change(delta_time_ms);
        return;
    }
}

void camera::set_animation(int animation)
{
    animation_locked = true;
    animation_timer = 0;

    if (animation == C_ANIMATION_IDLE)
    {
        current_animation = C_ANIMATION_IDLE;
        animation_locked = false;
        return;
    }

    if (animation == C_ANIMATION_CAMERA_LOCKING)
    {
        start_lock_point = pos + dir;

        is_free_camera = false;
        current_animation = C_ANIMATION_CAMERA_LOCKING;
        return;
    }

    if (animation == C_ANIMATION_CHANGE_TARGET)
    {
        start_lock_point = pos + dir;
        start_pos = pos;

        alpha = 0;
        beta = 1 / 4.0f * M_PI;
        radius = target_radius;

        is_free_camera = false;
        spherical_to_cartesian_coords(true);

        current_animation = C_ANIMATION_CHANGE_TARGET;
        return;
    }

    if (animation == C_ANIMATION_CHANGE_RADIUS)
    {
        start_radius = radius;

        current_animation = C_ANIMATION_CHANGE_RADIUS;
        return;
    }
}

// animation functions

void camera::animate_camera_locking(int delta_time_ms)
{
    animation_timer += delta_time_ms;

    if (animation_timer >= C_DURATION_CAMERA_LOCKING)
    {
        set_animation(C_ANIMATION_IDLE);
        lock_point = target_lock_point;

        return;
    }

    float time_alpha = (float)animation_timer / (float)C_DURATION_CAMERA_LOCKING;
    float eased_alpha = time_alpha * time_alpha * (3.0f - 2.0f * time_alpha);

    lock_point = start_lock_point + (target_lock_point - start_lock_point) * eased_alpha;

    spherical_to_cartesian_coords();

    glutPostRedisplay();
}

void camera::animate_changing_target(int delta_time_ms)
{
    animation_timer += delta_time_ms;

    if (animation_timer >= C_DURATION_CHANGE_TARGET)
    {
        set_animation(C_ANIMATION_IDLE);

        lock_point = target_lock_point;

        spherical_to_cartesian_coords();

        return;
    }

    float time_alpha = (float)animation_timer / (float)C_DURATION_CHANGE_TARGET;

    float eased_alpha = time_alpha * time_alpha * (3.0f - 2.0f * time_alpha);

    if (eased_alpha <= 0.5f)
    {
        lock_point = start_lock_point + (target_lock_point - start_lock_point) * eased_alpha * 2.0f;
    }
    pos = start_pos + (target_pos - start_pos) * eased_alpha;

    glutPostRedisplay();
}

void camera::animate_radius_change(int delta_time_ms)
{
    animation_timer += delta_time_ms;

    if (animation_timer >= C_DURATION_CHANGE_RADIUS)
    {
        set_animation(C_ANIMATION_IDLE);

        radius = target_radius;

        spherical_to_cartesian_coords();

        return;
    }

    float time_alpha = (float)animation_timer / (float)C_DURATION_CHANGE_RADIUS;

    float eased_alpha = time_alpha * time_alpha * (3.0f - 2.0f * time_alpha);

    radius = start_radius + (target_radius - start_radius) * eased_alpha;

    spherical_to_cartesian_coords();

    glutPostRedisplay();
}

// render important

matrix4x4 camera::get_view_matrix()
{
    if (is_free_camera)
    {
        lock_point = pos + dir;
    }
    return matrix4x4::View(pos, lock_point, up);
}

// AUXILIARY FUNCTIONS

void camera::cartesian_to_spherical_coords(bool set_target_radius)
{
    vector3 rel_pos;

    rel_pos = pos - target_lock_point;

    radius = sqrt(rel_pos.x * rel_pos.x + rel_pos.y * rel_pos.y + rel_pos.z * rel_pos.z);
    beta = asin(rel_pos.y / radius);
    alpha = atan2(rel_pos.x, rel_pos.z);

    target_radius = radius;
}

void camera::spherical_to_cartesian_coords(bool to_target)
{
    vector3 rel_pos;

    rel_pos.x = radius * cos(beta) * sin(alpha);
    rel_pos.y = radius * sin(beta);
    rel_pos.z = radius * cos(beta) * cos(alpha);

    if (to_target)
    {
        target_pos = rel_pos + target_lock_point;

        return;
    }

    pos = rel_pos + target_lock_point;
    dir = target_lock_point - pos;

    dir.normalize();
}

void camera::print_info()
{
    std::cout << "Position: " << pos.x << " " << pos.y << " " << pos.z << "\n";
    std::cout << "Lock point: " << target_lock_point.x << " " << target_lock_point.y << " " << target_lock_point.z << "\n";
    std::cout << "Up vector: " << up.x << " " << up.y << " " << up.z << "\n";
    std::cout << "Direction vector: " << dir.x << " " << dir.y << " " << dir.z << "\n";
    std::cout << "Spherical coords: " << radius << " " << alpha << " " << beta << "\n";
    std::cout << "Pitch/Yaw: " << pitch << " " << yaw << "\n-|- >< \n\n"
              << std::endl;
}

float camera::radian_to_degree(float ang_r)
{
    return ang_r * (180.0f / M_PI);
}

bool camera::update_frustum()
{
    return !is_free_camera;
}