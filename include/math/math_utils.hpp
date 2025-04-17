#ifndef MATH_UTILS_HPP
#define MATH_UTILS_HPP

#include "math/vector3.hpp"
#include "math/vector4.hpp"
#include "math/matrix4x4.hpp"

namespace math_utils
{
    vector3 point_on_bezier(float t, vector3 p0, vector3 p1, vector3 p2, vector3 p3);
    vector3 derivative_on_bezier(float t, vector3 p0, vector3 p1, vector3 p2, vector3 p3);
}

#endif