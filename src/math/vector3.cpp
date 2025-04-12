#include "math/vector3.hpp"

vector3::vector3()
{
    x = 0;
    y = 0;
    z = 0;
}

vector3::vector3(float x, float y, float z)
{
    this->x = x;
    this->y = y;
    this->z = z;
}

// vector specific math

float vector3::dot(const vector3 &v1, const vector3 &v2)
{
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

vector3 vector3::cross(const vector3 &v1, const vector3 &v2)
{
    return vector3(
        v1.y * v2.z - v1.z * v2.y,
        v1.z * v2.x - v1.x * v2.z,
        v1.x * v2.y - v1.y * v2.x);
}

float vector3::magnitude() const
{
    return sqrtf(x * x + y * y + z * z);
}

void vector3::normalize()
{
    float len = sqrtf(x * x + y * y + z * z);

    x /= len;
    y /= len;
    z /= len;
}

// operator(s)

vector3 vector3::operator+(const vector3 &other) const
{
    vector3 result;

    result.x = x + other.x;
    result.y = y + other.y;
    result.z = z + other.z;

    return result;
}

vector3 vector3::operator-(const vector3 &other) const
{
    vector3 result;

    result.x = x - other.x;
    result.y = y - other.y;
    result.z = z - other.z;

    return result;
}

vector3 vector3::operator*(const float &scalar) const
{
    vector3 result;

    result.x = x * scalar;
    result.y = y * scalar;
    result.z = z * scalar;

    return result;
}

vector3 vector3::operator/(const float &scalar) const
{
    vector3 result;

    result.x = x / scalar;
    result.y = y / scalar;
    result.z = z / scalar;

    return result;
}

vector3 &vector3::operator+=(const vector3 &other)
{
    x += other.x;
    y += other.y;
    z += other.z;

    return *this;
}

vector3 &vector3::operator-=(const vector3 &other)
{
    x -= other.x;
    y -= other.y;
    z -= other.z;

    return *this;
}

vector3 &vector3::operator*=(const float &other)
{
    x *= other;
    y *= other;
    z *= other;

    return *this;
}

vector3 &vector3::operator/=(const float &other)
{
    x /= other;
    y /= other;
    z /= other;

    return *this;
}

std::ostream &operator<<(std::ostream &os, const vector3 &vec)
{
    os << vec.x << ";" << vec.y << ";" << vec.z;
    return os;
}
