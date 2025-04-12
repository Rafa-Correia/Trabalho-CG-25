#ifndef PLANE_HPP
#define PLANE_HPP

#include "generator/shape_generator.hpp"

class plane_generator : public shape_generator
{
public:
    void generate(int argc, char **argv) override;
};

#endif