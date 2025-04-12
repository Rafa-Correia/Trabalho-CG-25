#ifndef TORUS_HPP
#define TORUS_HPP

#include "generator/shape_generator.hpp"

class torus_generator : public shape_generator
{
public:
    void generate(int argc, char **argv) const override;
};

#endif