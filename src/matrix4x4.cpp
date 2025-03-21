#include "matrix4x4.hpp"
 
matrix4x4::matrix4x4() {
    for(int i = 0; i < 16; i++) {
        m_data[i] = 0;
    }

    m_data[0] = 1;
    m_data[5] = 1;
    m_data[10] = 1;
    m_data[15] = 1;
}

matrix4x4 matrix4x4::Identity() {
    return *(new matrix4x4());
}

matrix4x4 matrix4x4::Translate(float x, float y, float z) {
    matrix4x4 result;

    result.m_data[12] = x;
    result.m_data[13] = y;
    result.m_data[14] = z;

    return result;
}

matrix4x4 matrix4x4::Rotate(float theta, float x, float y, float z) {
    matrix4x4 result;
    float c = cos(theta);
    float s = sin(theta);
    float oneMinusC = 1.0f - c;

    // Normalize axis
    float len = sqrt(x*x + y*y + z*z);
    if (len == 0.0f) return result;  // Return identity if invalid axis
    x /= len; y /= len; z /= len;

    result.m_data[0]  = c + x*x*oneMinusC;
    result.m_data[1]  = y*x*oneMinusC + z*s;
    result.m_data[2]  = z*x*oneMinusC - y*s;
    result.m_data[3]  = 0.0f;

    result.m_data[4]  = x*y*oneMinusC - z*s;
    result.m_data[5]  = c + y*y*oneMinusC;
    result.m_data[6]  = z*y*oneMinusC + x*s;
    result.m_data[7]  = 0.0f;

    result.m_data[8]  = x*z*oneMinusC + y*s;
    result.m_data[9]  = y*z*oneMinusC - x*s;
    result.m_data[10] = c + z*z*oneMinusC;
    result.m_data[11] = 0.0f;

    result.m_data[12] = result.m_data[13] = result.m_data[14] = 0.0f;
    result.m_data[15] = 1.0f;

    return result;
}

matrix4x4 matrix4x4::Scale(float x, float y, float z) {
    matrix4x4 result;

    result.m_data[0] = x;
    result.m_data[5] = y;
    result.m_data[10] = z;

    return result;
}

matrix4x4 matrix4x4::operator*(const matrix4x4& other) const {
    matrix4x4 result;
    int row, col;

    for(row = 0; row < 4; row++) {
        for(col = 0; col < 4; col++) {
            result.m_data[col*4 + row] = 
                m_data[0*4 + row] * other.m_data[col*4 + 0] +
                m_data[1*4 + row] * other.m_data[col*4 + 1] +
                m_data[2*4 + row] * other.m_data[col*4 + 2] +
                m_data[3*4 + row] * other.m_data[col*4 + 3];
        }
    }

    return result;
}

vector3 matrix4x4::apply_to_point(float x, float y, float z) {
    float new_x, new_y, new_z;
    new_x = m_data[0]*x + m_data[4]*y + m_data[8]*z + m_data[12];
    new_y = m_data[1]*x + m_data[5]*y + m_data[9]*z + m_data[13];
    new_z = m_data[2]*x + m_data[6]*y + m_data[10]*z + m_data[14];

    return vector3(new_x, new_y, new_z);
}

float* matrix4x4::get_data() {
    return this->m_data;
}