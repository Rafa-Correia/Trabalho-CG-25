#ifndef MATRIX4X4_HPP
#define MATRIX4X4_HPP

#define _USE_MATH_DEFINES
#include <math.h>

#include <tuple>

class matrix4x4 {
    public:
        matrix4x4();

        static matrix4x4 Identity();
        static matrix4x4 Translate(float x, float y, float z);
        static matrix4x4 Rotate(float theta, float x, float y, float z);
        static matrix4x4 Scale(float x, float y, float z);

        matrix4x4 operator * (const matrix4x4& other) const;

        float *get_data();
        std::tuple<float, float, float> apply_to_point(float x, float y, float z);
    private:
        float m_data[16];

        matrix4x4(float x, float y, float z);
};

#endif