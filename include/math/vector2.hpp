#ifndef VECTOR2_HPP
#define VECTOR2_HPP

#include <iostream>

#define _USE_MATH_DEFINES
#include <math.h>

class vector2
{
public:
    float x = 0, y = 0;

    /**
     * Empty constructor. Returns vector with value 0 magnitude.
     */
    vector2();

    /**
     * Creates vector with given components.
     *
     * @param x x component of vector.
     * @param y y component of vector.
     */
    vector2(float x, float y);

    friend std::ostream &operator<<(std::ostream &os, const vector2 &vec);

private:
};

#endif