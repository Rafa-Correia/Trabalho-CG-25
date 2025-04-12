#ifndef TRANSLATION_DYNAMIC_HPP
#define TRANSLATION_DYNAMIC_HPP

#include "translation.hpp"

class translation_dynamic : public translation
{
public:
    translation_dynamic(float total_time, bool align, std::vector<vector3> points, bool loop, int path_divisions = PATH_DIVISIONS);

    void update(int delta_time_ms) override;
    matrix4x4 get_translation() override;
    void draw_path() override;

    operator const matrix4x4() const override;

private:
    float total_time;
    bool align = false;
    bool loop = true;
    int animation_timer;

    std::vector<vector3> points;
    matrix4x4 translation_matrix;

    std::vector<vector3> path_points;
    std::vector<vector3> path_derivs;

    // needs to return derivative as well, to align and allat
    std::tuple<vector3, vector3> p_d_on_curve(float time_alpha);
    std::tuple<vector3, vector3> p_d_on_segment(float segment_time_alpha, vector3 p0, vector3 p1, vector3 p2, vector3 p3);
};

#endif