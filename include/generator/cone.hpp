#ifndef CONE_HPP
#define CONE_HPP

#include "generator/shape_generator.hpp"

class cone_generator : public shape_generator
{
public:
    void generate(int argc, char **argv) override;
};

#endif