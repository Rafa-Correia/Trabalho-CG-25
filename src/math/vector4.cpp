#include "math/vector4.hpp"

vector4::vector4()
{
    x = 0;
    y = 0;
    z = 0;
}

vector4::vector4(float x, float y, float z, float w)
{
    this->x = x;
    this->y = y;
    this->z = z;
    this->w = w;
}

// vector specific math

float vector4::magnitude() const
{
    return sqrtf(x * x + y * y + z * z + w * w);
}

void vector4::normalize(bool include_w)
{
    if (include_w)
    {
        float len = sqrtf(x * x + y * y + z * z + w * w);

        x /= len;
        y /= len;
        z /= len;
        w /= len;
    }
    else
    {
        float len = sqrtf(x * x + y * y + z * z);

        x /= len;
        y /= len;
        z /= len;
        w /= len;
    }
}

// operator(s)

vector4 vector4::operator+(const vector4 &other) const
{
    vector4 result;

    result.x = x + other.x;
    result.y = y + other.y;
    result.z = z + other.z;
    result.w = w + other.w;

    return result;
}

vector4 vector4::operator-(const vector4 &other) const
{
    vector4 result;

    result.x = x - other.x;
    result.y = y - other.y;
    result.z = z - other.z;
    result.w = w - other.w;

    return result;
}

vector4 vector4::operator*(const float &scalar) const
{
    vector4 result;

    result.x = x * scalar;
    result.y = y * scalar;
    result.z = z * scalar;
    result.w = w * scalar;

    return result;
}

vector4 vector4::operator/(const float &scalar) const
{
    vector4 result;

    result.x = x / scalar;
    result.y = y / scalar;
    result.z = z / scalar;
    result.w = w / scalar;

    return result;
}

vector4 &vector4::operator+=(const vector4 &other)
{
    x += other.x;
    y += other.y;
    z += other.z;
    w += other.w;

    return *this;
}

vector4 &vector4::operator-=(const vector4 &other)
{
    x -= other.x;
    y -= other.y;
    z -= other.z;
    w -= other.w;

    return *this;
}

vector4 &vector4::operator*=(const float &scalar)
{
    x *= scalar;
    y *= scalar;
    z *= scalar;
    w *= scalar;

    return *this;
}

vector4 &vector4::operator/=(const float &scalar)
{
    x /= scalar;
    y /= scalar;
    z /= scalar;
    w /= scalar;

    return *this;
}

float vector4::operator*(const vector4 &other) const
{
    float result = 0;

    result += this->x * other.x;
    result += this->y * other.y;
    result += this->z * other.z;
    result += this->w * other.w;

    return result;
}