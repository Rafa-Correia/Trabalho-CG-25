#include "transforms/rotation_static.hpp"

rotation_static::rotation_static(float angle, vector3 rotation_vector) {
    float theta = angle * (M_PI / 180.0f);
    this->rotation_matrix = matrix4x4::Rotate(theta, rotation_vector);
}

matrix4x4 rotation_static::get_rotation() {
    return this->rotation_matrix;
}