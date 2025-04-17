#include "math/math_utils.hpp"

namespace math_utils
{
    vector3 point_on_bezier(float t, vector3 p0, vector3 p1, vector3 p2, vector3 p3)
    {
        static matrix4x4 m = matrix4x4::Bezier();
        // something
        vector4 p_x(p0.x, p1.x, p2.x, p3.x);
        vector4 p_y(p0.y, p1.y, p2.y, p3.y);
        vector4 p_z(p0.z, p1.z, p2.z, p3.z);

        vector4 a_x = m * p_x;
        vector4 a_y = m * p_y;
        vector4 a_z = m * p_z;

        vector4 t_v(t * t * t, t * t, t, 1);

        vector3 pos(t_v * a_x, t_v * a_y, t_v * a_z);

        return pos;
    }

    vector3 derivative_on_bezier(float t, vector3 p0, vector3 p1, vector3 p2, vector3 p3)
    {
        static matrix4x4 m = matrix4x4::Bezier();
        // something
        vector4 p_x(p0.x, p1.x, p2.x, p3.x);
        vector4 p_y(p0.y, p1.y, p2.y, p3.y);
        vector4 p_z(p0.z, p1.z, p2.z, p3.z);

        vector4 a_x = m * p_x;
        vector4 a_y = m * p_y;
        vector4 a_z = m * p_z;

        vector4 td_v(3 * t * t, 2 * t, 1, 0);

        vector3 deriv(td_v * a_x, td_v * a_y, td_v * a_z);

        return deriv;
    }
}