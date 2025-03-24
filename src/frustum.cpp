#include "frustum.hpp"

frustum::frustum(matrix4x4 projection_view_matrix) {
    float left[4], right[4], top[4], bottom[4], near[4], far[4];

    for (int i = 4; i--; ) { left[i]   = projection_view_matrix.get_data_at_point(i, 3) + projection_view_matrix.get_data_at_point(i, 0); }
    for (int i = 4; i--; ) { right[i]  = projection_view_matrix.get_data_at_point(i, 3) - projection_view_matrix.get_data_at_point(i, 0); }
    for (int i = 4; i--; ) { bottom[i] = projection_view_matrix.get_data_at_point(i, 3) + projection_view_matrix.get_data_at_point(i, 1); }
    for (int i = 4; i--; ) { top[i]    = projection_view_matrix.get_data_at_point(i, 3) - projection_view_matrix.get_data_at_point(i, 1); }
    for (int i = 4; i--; ) { near[i]   = projection_view_matrix.get_data_at_point(i, 3) + projection_view_matrix.get_data_at_point(i, 2); }
    for (int i = 4; i--; ) { far[i]    = projection_view_matrix.get_data_at_point(i, 3) - projection_view_matrix.get_data_at_point(i, 2); }

    left_plane = vector4(left[0], left[1], left[2], left[3]);
    right_plane = vector4(right[0], right[1], right[2], right[3]);
    top_plane = vector4(top[0], top[1], top[2], top[3]);
    bottom_plane = vector4(bottom[0], bottom[1], bottom[2], bottom[3]);
    near_plane = vector4(near[0], near[1], near[2], near[3]);
    far_plane = vector4(far[0], far[1], far[2], far[3]);
}

bool frustum::inside_frustum(vector3 position, float radius) {
    //var dist = dot3(world_space_point.xyz, p_planes[i].xyz) + p_planes[i].d + sphere_radius;
    float dist = vector3::dot(position, vector3(left_plane.x, left_plane.y, left_plane.z)) + left_plane.w + radius;
    if(dist < 0) return false; // sphere culled

    dist = vector3::dot(position, vector3(right_plane.x, right_plane.y, right_plane.z)) + right_plane.w + radius;
    if(dist < 0) return false; // sphere culled

    dist = vector3::dot(position, vector3(top_plane.x, top_plane.y, top_plane.z)) + top_plane.w + radius;
    if(dist < 0) return false; // sphere culled

    dist = vector3::dot(position, vector3(bottom_plane.x, bottom_plane.y, bottom_plane.z)) + bottom_plane.w + radius;
    if(dist < 0) return false; // sphere culled

    dist = vector3::dot(position, vector3(near_plane.x, near_plane.y, near_plane.z)) + near_plane.w + radius;
    if(dist < 0) return false; // sphere culled

    dist = vector3::dot(position, vector3(far_plane.x, far_plane.y, far_plane.z)) + far_plane.w + radius;
    if(dist < 0) return false; // sphere culled

    return true;
}

void frustum::draw_frustum() {
    vector4 planes[6] = {
        left_plane,
        right_plane,
        bottom_plane,
        top_plane,
        near_plane,
        far_plane
    };

    draw_frustum_private(planes);
}

vector3 frustum::intersect_planes(const vector4& p1, const vector4& p2, const vector4& p3) {
    vector3 n1(p1.x, p1.y, p1.z);
    vector3 n2(p2.x, p2.y, p2.z);
    vector3 n3(p3.x, p3.y, p3.z);

    vector3 cross23 = vector3::cross(n2, n3);
    vector3 cross31 = vector3::cross(n3, n1);
    vector3 cross12 = vector3::cross(n1, n2);

    float denom = vector3::dot(n1, cross23);
    if (denom == 0.0f) return vector3(0,0,0); // Planes don't intersect at a point

    vector3 result = (
        cross23 * (-p1.w) +
        cross31 * (-p2.w) +
        cross12 * (-p3.w)
    ) / denom;

    return result;
}

void frustum::draw_frustum_private(const vector4 planes[6]) {
    // Plane indices
    const int LEFT = 0, RIGHT = 1, BOTTOM = 2, TOP = 3, NEAR = 4, FAR = 5;

    // Compute 8 corners
    vector3 ntl = intersect_planes(planes[NEAR], planes[TOP],    planes[LEFT]);
    vector3 ntr = intersect_planes(planes[NEAR], planes[TOP],    planes[RIGHT]);
    vector3 nbl = intersect_planes(planes[NEAR], planes[BOTTOM], planes[LEFT]);
    vector3 nbr = intersect_planes(planes[NEAR], planes[BOTTOM], planes[RIGHT]);
    vector3 ftl = intersect_planes(planes[FAR],  planes[TOP],    planes[LEFT]);
    vector3 ftr = intersect_planes(planes[FAR],  planes[TOP],    planes[RIGHT]);
    vector3 fbl = intersect_planes(planes[FAR],  planes[BOTTOM], planes[LEFT]);
    vector3 fbr = intersect_planes(planes[FAR],  planes[BOTTOM], planes[RIGHT]);

    glColor3f(1.0f, 1.0f, 0.0f); // Yellow frustum lines
        glBegin(GL_LINES);
        // Near plane
        glVertex3f(ntl.x, ntl.y, ntl.z); glVertex3f(ntr.x, ntr.y, ntr.z);
        glVertex3f(ntr.x, ntr.y, ntr.z); glVertex3f(nbr.x, nbr.y, nbr.z);
        glVertex3f(nbr.x, nbr.y, nbr.z); glVertex3f(nbl.x, nbl.y, nbl.z);
        glVertex3f(nbl.x, nbl.y, nbl.z); glVertex3f(ntl.x, ntl.y, ntl.z);

        // Far plane
        glVertex3f(ftl.x, ftl.y, ftl.z); glVertex3f(ftr.x, ftr.y, ftr.z);
        glVertex3f(ftr.x, ftr.y, ftr.z); glVertex3f(fbr.x, fbr.y, fbr.z);
        glVertex3f(fbr.x, fbr.y, fbr.z); glVertex3f(fbl.x, fbl.y, fbl.z);
        glVertex3f(fbl.x, fbl.y, fbl.z); glVertex3f(ftl.x, ftl.y, ftl.z);

        // Connect near and far
        glVertex3f(ntl.x, ntl.y, ntl.z); glVertex3f(ftl.x, ftl.y, ftl.z);
        glVertex3f(ntr.x, ntr.y, ntr.z); glVertex3f(ftr.x, ftr.y, ftr.z);
        glVertex3f(nbl.x, nbl.y, nbl.z); glVertex3f(fbl.x, fbl.y, fbl.z);
        glVertex3f(nbr.x, nbr.y, nbr.z); glVertex3f(fbr.x, fbr.y, fbr.z);
    glEnd();
}