#ifndef ROTATION_HPP
#define ROTATION_HPP

#include "vector3.hpp"
#include "matrix4x4.hpp"

#define _USE_MATH_DEFINES
#include <math.h>

class rotation
{
public:
    virtual void update(int delta_time_ms) = 0;
    virtual matrix4x4 get_rotation() = 0;

    virtual operator const matrix4x4() const = 0;
};

#endif