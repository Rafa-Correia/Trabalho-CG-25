#include "engine/transforms/rotation_dynamic.hpp"

rotation_dynamic::rotation_dynamic(float time, vector3 rotation_vector)
{
    this->full_time = time;
    this->rotation_vector = rotation_vector;

    this->animation_counter = 0;
}

void rotation_dynamic::update(int delta_time_ms)
{
    this->animation_counter += delta_time_ms;
    float time_alpha = ((float)animation_counter / 1000.0f) / full_time;
    float angle = time_alpha * (2 * (float)M_PI);

    if (time_alpha >= 1.0f)
        animation_counter = 0;

    this->rotation_matrix = matrix4x4::Rotate(angle, this->rotation_vector);
}

matrix4x4 rotation_dynamic::get_rotation()
{
    return this->rotation_matrix;
}

rotation_dynamic::operator const matrix4x4() const
{
    return this->rotation_matrix;
}