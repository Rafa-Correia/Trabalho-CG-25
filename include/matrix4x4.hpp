#ifndef MATRIX4X4_HPP
#define MATRIX4X4_HPP

#include "vector3.hpp"
#include "vector4.hpp"
#include "printer.hpp"

#include <vector>
#include <string>
#include <iostream>
#include <sstream>

#ifdef _WIN32
#include <windows.h>
#endif

#define _USE_MATH_DEFINES
#include <math.h>

class matrix4x4
{
public:
    /**
     * Empty constructor.
     *
     * @returns Identity matrix representation.
     */
    matrix4x4();

    /**
     * Creates matrix from float array content.
     */
    matrix4x4(std::vector<float> content);

    /**
     * Creates Identity 4 by 4 matrix.
     *
     * @returns Identity matrix representation.
     */
    static matrix4x4 Identity();

    /**
     * Creates transformation matrix for a translation.
     *
     * @param translation_vector Vector to translate by.
     *
     * @returns Representation of 4 by 4 translation matrix.
     */
    static matrix4x4 Translate(vector3 translation_vector);

    /**
     * Creates transformation matrix for a rotation.
     *
     * @param theta Angle to rotate around the rotation vector. In radians.
     * @param rotation_vector Vector to rotate around.
     *
     * @returns Representation of 4 by 4 rotation matrix.
     */
    static matrix4x4 Rotate(float theta, vector3 rotation_vector);

    /**
     * Creates transformation matrix for scaling.
     *
     * @param scale_vector Vector containing scaling information.
     *
     * @returns Representation of 4 by 4 scaling matrix.
     */
    static matrix4x4 Scale(vector3 scale_vector);

    /**
     * Creates view matrix from camera parameters.
     *
     * @param eye Camera position (eye).
     * @param center Camera target (center).
     * @param up Camera's up vector.
     *
     * @returns Representation of 4 by 4 View matrix.
     */
    static matrix4x4 View(vector3 eye, vector3 center, vector3 up);

    /**
     * Creates projection matrix from projection attributes.
     *
     * @param fov Field of view.
     * @param aspect_ratio Aspect ratio of viewport.
     * @param near_plane Near plane distance.
     * @param far_plane Far plane distance.
     */
    static matrix4x4 Projection(float fov, float aspect_ratio, float near_plane, float far_plane);

    /**
     * Creates matrix for catmul rom curve calcs.
     */
    static matrix4x4 Catmul_rom();

    /**
     * Getter for matrix data in specific position.
     *
     * @param row Row of intended value.
     * @param column Column of intented value.
     *
     * @returns Value at row and column.
     */
    float get_data_at_point(int row, int column);

    matrix4x4 operator*(const matrix4x4 &other) const;
    vector3 operator*(const vector3 &vec) const; // i think this is unused, anyway, this is the same as multiplying by a vec4 with w = 1
    vector4 operator*(const vector4 &vec) const;

    operator const float *() const;

private:
    float m_data[16]; // < -- Inner structure storing matrix data.
};

#endif