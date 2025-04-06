#ifndef MATRIX4X4_HPP
#define MATRIX4X4_HPP

#include "vector3.hpp"

#define _USE_MATH_DEFINES
#include <math.h>

#include <tuple>

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
     * Getter for matrix data, in a float[16] representation.
     *
     * @returns Matrix data in a float[16] format. Always has 16 elements.
     */
    float *get_data();

    /**
     * Getter for matrix data in specific position.
     *
     * @param row Row of intended value.
     * @param column Column of intented value.
     *
     * @returns Value at row and column.
     */
    float get_data_at_point(int row, int column);

    /**
     * Applies transformation to a point in 3D space.
     *
     * @param x x component of point.
     * @param y y component of point.
     * @param z z component of point.
     */
    vector3 apply_to_point(vector3 point);

    matrix4x4 operator*(const matrix4x4 &other) const;

private:
    float m_data[16]; // < -- Inner structure storing matrix data.
};

#endif