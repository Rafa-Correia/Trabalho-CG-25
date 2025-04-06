#ifndef TRANSLATION_DYNAMIC_HPP
#define TRANSLATION_DYNAMIC_HPP

#include "translation.hpp"

class translation_dynamic : public translation {
    public:
        translation_dynamic(float total_time, bool align, std::vector<vector3> points, int path_divisions = 50);

        void update(int delta_time_ms) override;
        matrix4x4 get_translation() override;

        void draw_path() override;
    private:
        float total_time;
        bool align = false;
        int animation_timer;

        std::vector<vector3> points;
        matrix4x4 translation_matrix;

        std::vector<vector3> path_points;
        std::vector<vector3> path_derivs;

        //needs to return derivative as well, to align and allat
        vector3 position_on_curve(float time_alpha);
        vector3 position_on_segment(float segment_time_alpha, vector3 p0, vector3 p1, vector3 p2, vector3 p3);

        static void multMatrixVector(float *m, float *v, float *res);
        static float * dataFromMatrix(float m[4][4]);
};

#endif