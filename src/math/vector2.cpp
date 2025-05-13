#include "math/vector2.hpp"

vector2::vector2()
{
    this->x = 0;
    this->y = 0;
}

vector2::vector2(float x, float y)
{
    this->x = x;
    this->y = y;
}

std::ostream &operator<<(std::ostream &os, const vector2 &vec)
{
    os << vec.x << ";" << vec.y;
    return os;
}
