#ifndef CYLINDER_HPP
#define CYLINDER_HPP

#include "generator/shape_generator.hpp"

class cylinder_generator : public shape_generator
{
public:
    void generate(int argc, char **argv) override;
};

#endif