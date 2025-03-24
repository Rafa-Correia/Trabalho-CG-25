#ifndef VECTOR3_HPP
#define VECTOR3_HPP

#define _USE_MATH_DEFINES
#include <math.h>

class vector3  {
    public:
        float x = 0, y = 0, z = 0;

        /**
         * Empty constructor. Returns vector with value 0 magnitude.
         */
        vector3();

        /**
         * Creates vector with given components. 
         * 
         * @param x x component of vector. 
         * @param y y component of vector. 
         * @param z z component of vector. 
         */
        vector3(float x, float y, float z);

        /**
         * Calculates dot product of two vectors.
         * 
         * @param v1 vector number 1. 
         * @param v2 vector number 2. 
         * 
         * @returns Value of dot product of the two given vectors.
         */
        static float dot(const vector3& v1, const vector3& v2);

        /**
         * Calculates cross product of two vectors. 
         * 
         * @param v1 vector number 1. 
         * @param v2 vector number 2. 
         * 
         * @returns Resulting vector of cross produt of the two given vectors.
         */
        static vector3 cross(const vector3& v1, const vector3& v2);
        
        /**
         * Calculates magnitude of vector.
         * 
         * @returns Magnitude of vector.
         */
        float magnitude() const;

        /**
         * Normalizes vector (so magnitude becomes 1).
         */
        void normalize();
        
        vector3 operator+(const vector3& other) const;
        vector3 operator-(const vector3& other) const;
        vector3 operator*(const float& scalar) const;
        vector3 operator/(const float& scalar) const;
        
        vector3& operator+=(const vector3& other);
        vector3& operator-=(const vector3& other);
        vector3& operator*=(const float& other);
        vector3& operator/=(const float& other);
    private:
};

#endif