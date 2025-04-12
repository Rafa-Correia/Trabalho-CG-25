#ifndef BOX_HPP
#define BOX_HPP

#include "generator/shape_generator.hpp"

class box_generator : public shape_generator
{
public:
    void generate(int argc, char **argv) override;
};

#endif