#ifndef TRANSLATION_STATIC_HPP
#define TRANSLATION_STATIC_HPP

#include "translation.hpp"

class translation_static : public translation {
    public:
        translation_static(vector3 translation_vector);

        void update(int delta_time_ms) override {return;}
        matrix4x4 get_translation() override;
        void draw_path() override {return;}
    private:
        matrix4x4 translation_matrix;
};

#endif