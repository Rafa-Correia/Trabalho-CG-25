#ifndef ROTATION_STATIC_HPP
#define ROTATION_STATIC_HPP

#include "engine/transforms/rotation.hpp"

class rotation_static : public rotation
{
public:
    rotation_static(float angle, vector3 rotation_vector);

    void update(int delta_time_ms) override { return; } // static rotation doesn't need to be updated
    matrix4x4 get_rotation() override;

    operator const matrix4x4() const override;

private:
    matrix4x4 rotation_matrix;
};

#endif