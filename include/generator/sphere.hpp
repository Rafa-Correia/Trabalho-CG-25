#ifndef SPHERE_HPP
#define SPHERE_HPP

#include "generator/shape_generator.hpp"

#define RIGHT_ANGLE (float)((1.0f / 2.0f) * M_PI)

class sphere_generator : public shape_generator
{
public:
    void generate(int argc, char **argv) override;
};

#endif