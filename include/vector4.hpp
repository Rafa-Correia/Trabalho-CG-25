#ifndef VECTOR4_HPP
#define VECTOR4_HPP

#include <iostream>

#define _USE_MATH_DEFINES
#include <math.h>

class vector4
{
public:
    float x = 0, y = 0, z = 0, w = 0;

    /**
     * Empty constructor. Returns vector with value 0 magnitude.
     */
    vector4();

    /**
     * Creates vector with given components.
     *
     * @param x x component of vector.
     * @param y y component of vector.
     * @param z z component of vector.
     * @param w w component of vector.
     */
    vector4(float x, float y, float z, float w);

    /**
     * Calculates magnitude of vector.
     *
     * @returns Magnitude of vector.
     */
    float magnitude() const;

    /**
     * Normalizes vector (so magnitude becomes 1).
     */
    void normalize(bool include_w = true);

    vector4 operator+(const vector4 &other) const;
    vector4 operator-(const vector4 &other) const;
    vector4 operator*(const float &scalar) const;
    vector4 operator/(const float &scalar) const;

    vector4 &operator+=(const vector4 &other);
    vector4 &operator-=(const vector4 &other);
    vector4 &operator*=(const float &scalar);
    vector4 &operator/=(const float &scalar);

private:
};

#endif