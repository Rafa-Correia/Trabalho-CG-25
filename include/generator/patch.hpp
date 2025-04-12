#ifndef PATCH_HPP
#define PATCH_HPP

#include "generator/shape_generator.hpp"

class patch_generator : public shape_generator
{
public:
    void generate(int argc, char **argv) override;

private:
    std::vector<std::vector<int>> patch_indices;
    std::vector<vector3> control_points;

    void parse_file(const std::string &filepath);
};

#endif