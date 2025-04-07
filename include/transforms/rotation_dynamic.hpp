#ifndef ROTATION_DYNAMIC_HPP
#define ROTATION_DYNAMIC_HPP

#include "rotation.hpp"

class rotation_dynamic : public rotation
{
public:
    rotation_dynamic(float time, vector3 rotation_vector);

    void update(int delta_time_ms) override;
    matrix4x4 get_rotation() override;

    operator const matrix4x4() const override;

private:
    float full_time;
    float animation_counter;

    vector3 rotation_vector;
    matrix4x4 rotation_matrix;
};

#endif