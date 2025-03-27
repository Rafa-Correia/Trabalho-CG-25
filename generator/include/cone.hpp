#ifndef CONE_HPP
#define CONE_HPP

#include "shape_generator.hpp"

class cone_generator : public shape_generator {
    public:
        void generate(int argc, char **argv) const override;
};

#endif