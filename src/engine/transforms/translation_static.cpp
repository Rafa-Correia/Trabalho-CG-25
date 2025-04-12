#include "engine/transforms/translation_static.hpp"

translation_static::translation_static(vector3 translation_vector)
{
    this->translation_matrix = matrix4x4::Translate(translation_vector);
}

matrix4x4 translation_static::get_translation()
{
    return this->translation_matrix;
}

translation_static::operator const matrix4x4() const
{
    return this->translation_matrix;
}