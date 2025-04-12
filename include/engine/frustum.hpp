#ifndef FRUSTUM_HPP
#define FRUSTUM_HPP

#include "math/vector3.hpp"
#include "math/vector4.hpp"
#include "math/matrix4x4.hpp"

#include <iostream>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/glut.h>
#endif

class frustum
{
public:
    frustum();

    /**
     * Checks if bounding sphere intersects view frustum.
     *
     * @param position Bounding sphere center.
     * @param radius Bounding sphere radius.
     *
     * @returns Boolean determining wether bounding sphere intersects view frustum.
     */
    bool inside_frustum(vector3 position, float radius);

    /**
     * Function responsible for updating frustum based on projection * view matrix.
     *
     * @param projection_view_matrix  Projection * View matrix for frustum extraction.
     */
    void update_frustum(matrix4x4 &projection_view_matrix);

    /**
     * Draws view frustum outline.
     */
    void draw_frustum();

private:
    vector4 left_plane;
    vector4 right_plane;
    vector4 top_plane;
    vector4 bottom_plane;
    vector4 near_plane;
    vector4 far_plane;

    static vector3 intersect_planes(const vector4 &p1, const vector4 &p2, const vector4 &p3);

    void draw_frustum_private(const vector4 planes[6]);
};

#endif