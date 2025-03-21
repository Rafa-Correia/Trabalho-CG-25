#ifndef MATRIX4X4_HPP
#define MATRIX4X4_HPP

#include "vector3.hpp"

#define _USE_MATH_DEFINES
#include <math.h>

#include <tuple>

class matrix4x4 {
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
         * @param x x component of translation vector. 
         * @param y y component of translation vector. 
         * @param z z component of translation vector. 
         * 
         * @returns Representation of 4 by 4 translation matrix. 
         */
        static matrix4x4 Translate(float x, float y, float z);

        /**
         * Creates transformation matrix for a rotation. 
         * 
         * @param theta Angle to rotate around the rotation vector. In radians. 
         * @param x x component of rotation vector. 
         * @param y y component of rotation vector. 
         * @param z z component of rotation vector. 
         * 
         * @returns Representation of 4 by 4 rotation matrix. 
         */
        static matrix4x4 Rotate(float theta, float x, float y, float z);

        /**
         * Creates transformation matrix for scaling. 
         * 
         * @param x x axis scaling. 
         * @param y y axis scaling. 
         * @param z z axis scaling. 
         * 
         * @returns Representation of 4 by 4 scaling matrix.
         */
        static matrix4x4 Scale(float x, float y, float z);
        
        /**
         * Getter for matrix data, in a float[16] representation. 
         * 
         * @returns Matrix data in a float[16] format. Always has 16 elements.
         */
        float *get_data();
        
        /**
         * Applies transformation to a point in 3D space. 
         * 
         * @param x x component of point. 
         * @param y y component of point. 
         * @param z z component of point.
         */
        vector3 apply_to_point(float x, float y, float z);


        matrix4x4 operator * (const matrix4x4& other) const;
    private:
        float m_data[16];       // < -- Inner structure storing matrix data. 
};

#endif