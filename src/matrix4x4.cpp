#include "matrix4x4.hpp"

matrix4x4::matrix4x4()
{
    for (int i = 0; i < 16; i++)
    {
        m_data[i] = 0;
    }

    m_data[0] = 1;
    m_data[5] = 1;
    m_data[10] = 1;
    m_data[15] = 1;
}

matrix4x4::matrix4x4(std::vector<float> content)
{
    if (content.size() != 16)
    {
        std::stringstream ss;

        ss << "Trying to build matrix with " << content.size() << " element(s). Matrix can only be built with 16 elements, returning identity...";
        printer::print_warning(ss.str());

        for (int i = 0; i < 16; i++)
        {
            this->m_data[i] = 0;
        }

        this->m_data[0] = 1;
        this->m_data[5] = 1;
        this->m_data[10] = 1;
        this->m_data[15] = 1;

        return;
    }

    for (size_t i = 0; i < 16; i++)
    {
        this->m_data[i] = content.at(i);
    }
}

// unique matrix builders

matrix4x4 matrix4x4::Identity()
{
    return matrix4x4();
}

matrix4x4 matrix4x4::Translate(vector3 translation_vector)
{
    matrix4x4 result;

    result.m_data[12] = translation_vector.x;
    result.m_data[13] = translation_vector.y;
    result.m_data[14] = translation_vector.z;

    return result;
}

matrix4x4 matrix4x4::Rotate(float theta, vector3 rotation_vector)
{
    matrix4x4 result;
    float x = rotation_vector.x, y = rotation_vector.y, z = rotation_vector.z;
    float c = cosf(theta);
    float s = sinf(theta);
    float oneMinusC = 1.0f - c;

    // Normalize axis
    float len = sqrtf(x * x + y * y + z * z);
    if (len == 0.0f)
        return result; // Return identity if invalid axis
    x /= len;
    y /= len;
    z /= len;

    result.m_data[0] = c + x * x * oneMinusC;
    result.m_data[1] = y * x * oneMinusC + z * s;
    result.m_data[2] = z * x * oneMinusC - y * s;
    result.m_data[3] = 0.0f;

    result.m_data[4] = x * y * oneMinusC - z * s;
    result.m_data[5] = c + y * y * oneMinusC;
    result.m_data[6] = z * y * oneMinusC + x * s;
    result.m_data[7] = 0.0f;

    result.m_data[8] = x * z * oneMinusC + y * s;
    result.m_data[9] = y * z * oneMinusC - x * s;
    result.m_data[10] = c + z * z * oneMinusC;
    result.m_data[11] = 0.0f;

    result.m_data[12] = result.m_data[13] = result.m_data[14] = 0.0f;
    result.m_data[15] = 1.0f;

    return result;
}

matrix4x4 matrix4x4::Scale(vector3 scale_vector)
{
    matrix4x4 result;

    result.m_data[0] = scale_vector.x;
    result.m_data[5] = scale_vector.y;
    result.m_data[10] = scale_vector.z;

    return result;
}

matrix4x4 matrix4x4::View(vector3 eye, vector3 center, vector3 up)
{
    vector3 F = center - eye;
    F.normalize();

    vector3 S = vector3::cross(F, up);
    S.normalize();

    vector3 U = vector3::cross(S, F);
    U.normalize();

    matrix4x4 result;

    // row 1
    result.m_data[0] = S.x;
    result.m_data[4] = S.y;
    result.m_data[8] = S.z;
    result.m_data[12] = 0.0f;

    // row 2
    result.m_data[1] = U.x;
    result.m_data[5] = U.y;
    result.m_data[9] = U.z;
    result.m_data[13] = 0.0f;

    // row 3
    result.m_data[2] = -(F.x);
    result.m_data[6] = -(F.y);
    result.m_data[10] = -(F.z);
    result.m_data[14] = 0.0f;

    // row 4
    result.m_data[3] = 0.0f;
    result.m_data[7] = 0.0f;
    result.m_data[11] = 0.0f;
    result.m_data[15] = 1.0f;

    result = result * matrix4x4::Translate(eye * -1.0f);

    return result;
}

matrix4x4 matrix4x4::Projection(float fov, float aspect_ratio, float near_plane, float far_plane)
{
    float fov_rad = fov * ((float)M_PI / 180.0f);
    float f = 1 / tanf(fov_rad / 2.0f);

    matrix4x4 result;
    // row - col (starting at 0)
    result.m_data[0] = f / aspect_ratio; // 00

    result.m_data[5] = f; // 11

    result.m_data[10] = (far_plane + near_plane) / (near_plane - far_plane); // 22
    result.m_data[11] = -1;                                                  // 32

    result.m_data[14] = (2 * far_plane * near_plane) / (near_plane - far_plane); // 23
    result.m_data[15] = 0;                                                       // 33

    return result;
}

matrix4x4 matrix4x4::Catmul_rom()
{
    matrix4x4 result;

    result.m_data[0] = -0.5f;
    result.m_data[1] = 1.0f;
    result.m_data[2] = -0.5f;
    result.m_data[3] = 0.0f;

    result.m_data[4] = 1.5f;
    result.m_data[5] = -2.5f;
    result.m_data[6] = 0.0f;
    result.m_data[7] = 1.0f;

    result.m_data[8] = -1.5f;
    result.m_data[9] = 2.0f;
    result.m_data[10] = 0.5f;
    result.m_data[11] = 0.0f;

    result.m_data[12] = 0.5f;
    result.m_data[13] = -0.5f;
    result.m_data[14] = 0.0f;
    result.m_data[15] = 0.0f;

    return result;
}

// getters

float matrix4x4::get_data_at_point(int row, int column)
{
    return m_data[row * 4 + column];
}

// operator(s)

matrix4x4 matrix4x4::operator*(const matrix4x4 &other) const
{
    matrix4x4 result;
    int row, col;

    for (row = 0; row < 4; row++)
    {
        for (col = 0; col < 4; col++)
        {
            result.m_data[col * 4 + row] =
                m_data[0 * 4 + row] * other.m_data[col * 4 + 0] +
                m_data[1 * 4 + row] * other.m_data[col * 4 + 1] +
                m_data[2 * 4 + row] * other.m_data[col * 4 + 2] +
                m_data[3 * 4 + row] * other.m_data[col * 4 + 3];
        }
    }

    return result;
}

vector3 matrix4x4::operator*(const vector3 &vec) const
{
    float new_x, new_y, new_z;
    new_x = m_data[0] * vec.x + m_data[4] * vec.y + m_data[8] * vec.z + m_data[12];
    new_y = m_data[1] * vec.x + m_data[5] * vec.y + m_data[9] * vec.z + m_data[13];
    new_z = m_data[2] * vec.x + m_data[6] * vec.y + m_data[10] * vec.z + m_data[14];

    return vector3(new_x, new_y, new_z);
}

vector4 matrix4x4::operator*(const vector4 &vec) const
{
    float new_x, new_y, new_z, new_w;
    new_x = m_data[0] * vec.x + m_data[4] * vec.y + m_data[8] * vec.z + m_data[12] * vec.w;
    new_y = m_data[1] * vec.x + m_data[5] * vec.y + m_data[9] * vec.z + m_data[13] * vec.w;
    new_z = m_data[2] * vec.x + m_data[6] * vec.y + m_data[10] * vec.z + m_data[14] * vec.w;
    new_w = m_data[3] * vec.x + m_data[7] * vec.y + m_data[11] * vec.z + m_data[15] * vec.w;

    return vector4(new_x, new_y, new_z, new_w);
}

matrix4x4::operator const float *() const
{
    return m_data;
}
