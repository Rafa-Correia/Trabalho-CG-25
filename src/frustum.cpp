#include "frustum.hpp"

frustum::frustum()
{
    left_plane = right_plane = top_plane = bottom_plane = near_plane = far_plane = vector4();
}

// collision check

bool frustum::inside_frustum(vector3 position, float radius)
{

    float dist = vector3::dot(position, vector3(left_plane.x, left_plane.y, left_plane.z)) + left_plane.w;
    if (dist + radius < 0)
        return false;

    dist = vector3::dot(position, vector3(right_plane.x, right_plane.y, right_plane.z)) + right_plane.w;
    if (dist + radius < 0)
        return false;

    dist = vector3::dot(position, vector3(top_plane.x, top_plane.y, top_plane.z)) + top_plane.w;
    if (dist + radius < 0)
        return false;

    dist = vector3::dot(position, vector3(bottom_plane.x, bottom_plane.y, bottom_plane.z)) + bottom_plane.w;
    if (dist + radius < 0)
        return false;

    dist = vector3::dot(position, vector3(near_plane.x, near_plane.y, near_plane.z)) + near_plane.w;
    if (dist + radius < 0)
        return false;

    dist = vector3::dot(position, vector3(far_plane.x, far_plane.y, far_plane.z)) + far_plane.w;
    if (dist + radius < 0)
        return false;

    return true;
}

// update

void frustum::update_frustum(matrix4x4 &projection_view_matrix)
{
    float left[4], right[4], top[4], bottom[4], near[4], far[4];

    for (int i = 4; i--;)
    {
        left[i] = projection_view_matrix.get_data_at_point(i, 3) + projection_view_matrix.get_data_at_point(i, 0);
    }
    for (int i = 4; i--;)
    {
        right[i] = projection_view_matrix.get_data_at_point(i, 3) - projection_view_matrix.get_data_at_point(i, 0);
    }
    for (int i = 4; i--;)
    {
        bottom[i] = projection_view_matrix.get_data_at_point(i, 3) + projection_view_matrix.get_data_at_point(i, 1);
    }
    for (int i = 4; i--;)
    {
        top[i] = projection_view_matrix.get_data_at_point(i, 3) - projection_view_matrix.get_data_at_point(i, 1);
    }
    for (int i = 4; i--;)
    {
        near[i] = projection_view_matrix.get_data_at_point(i, 3) + projection_view_matrix.get_data_at_point(i, 2);
    }
    for (int i = 4; i--;)
    {
        far[i] = projection_view_matrix.get_data_at_point(i, 3) - projection_view_matrix.get_data_at_point(i, 2);
    }

    left_plane = vector4(left[0], left[1], left[2], left[3]);
    right_plane = vector4(right[0], right[1], right[2], right[3]);
    top_plane = vector4(top[0], top[1], top[2], top[3]);
    bottom_plane = vector4(bottom[0], bottom[1], bottom[2], bottom[3]);
    near_plane = vector4(near[0], near[1], near[2], near[3]);
    far_plane = vector4(far[0], far[1], far[2], far[3]);

    left_plane.normalize(false);
    right_plane.normalize(false);
    top_plane.normalize(false);
    bottom_plane.normalize(false);
    near_plane.normalize(false);
    far_plane.normalize(false);
}

// frustum drawing (purely for debug)

void frustum::draw_frustum()
{
    vector4 planes[6] = {
        left_plane,
        right_plane,
        bottom_plane,
        top_plane,
        near_plane,
        far_plane};

    draw_frustum_private(planes);
}

vector3 frustum::intersect_planes(const vector4 &p1, const vector4 &p2, const vector4 &p3)
{
    vector3 n1(p1.x, p1.y, p1.z);
    vector3 n2(p2.x, p2.y, p2.z);
    vector3 n3(p3.x, p3.y, p3.z);

    vector3 cross23 = vector3::cross(n2, n3);
    vector3 cross31 = vector3::cross(n3, n1);
    vector3 cross12 = vector3::cross(n1, n2);

    float denom = vector3::dot(n1, cross23);
    if (denom == 0.0f)
        return vector3(0, 0, 0);

    vector3 result = (cross23 * (-p1.w) +
                      cross31 * (-p2.w) +
                      cross12 * (-p3.w)) /
                     denom;

    return result;
}

void frustum::draw_frustum_private(const vector4 planes[6])
{
    const int left = 0, right = 1, bottom = 2, top = 3, near = 4, far = 5;

    // frustum corners
    vector3 near_top_left = intersect_planes(planes[near], planes[top], planes[left]);
    vector3 near_top_right = intersect_planes(planes[near], planes[top], planes[right]);
    vector3 near_bottom_left = intersect_planes(planes[near], planes[bottom], planes[left]);
    vector3 near_bottom_right = intersect_planes(planes[near], planes[bottom], planes[right]);
    vector3 far_top_left = intersect_planes(planes[far], planes[top], planes[left]);
    vector3 far_top_right = intersect_planes(planes[far], planes[top], planes[right]);
    vector3 far_bottom_left = intersect_planes(planes[far], planes[bottom], planes[left]);
    vector3 far_bottom_right = intersect_planes(planes[far], planes[bottom], planes[right]);

    glColor3f(1.0f, 1.0f, 0.0f);
    glBegin(GL_LINES);
    // near plane
    glVertex3f(near_top_left.x, near_top_left.y, near_top_left.z);
    glVertex3f(near_top_right.x, near_top_right.y, near_top_right.z);

    glVertex3f(near_top_right.x, near_top_right.y, near_top_right.z);
    glVertex3f(near_bottom_right.x, near_bottom_right.y, near_bottom_right.z);

    glVertex3f(near_bottom_right.x, near_bottom_right.y, near_bottom_right.z);
    glVertex3f(near_bottom_left.x, near_bottom_left.y, near_bottom_left.z);

    glVertex3f(near_bottom_left.x, near_bottom_left.y, near_bottom_left.z);
    glVertex3f(near_top_left.x, near_top_left.y, near_top_left.z);

    // far plane
    glVertex3f(far_top_left.x, far_top_left.y, far_top_left.z);
    glVertex3f(far_top_right.x, far_top_right.y, far_top_right.z);

    glVertex3f(far_top_right.x, far_top_right.y, far_top_right.z);
    glVertex3f(far_bottom_right.x, far_bottom_right.y, far_bottom_right.z);

    glVertex3f(far_bottom_right.x, far_bottom_right.y, far_bottom_right.z);
    glVertex3f(far_bottom_left.x, far_bottom_left.y, far_bottom_left.z);

    glVertex3f(far_bottom_left.x, far_bottom_left.y, far_bottom_left.z);
    glVertex3f(far_top_left.x, far_top_left.y, far_top_left.z);

    // connect near and far planes
    glVertex3f(near_top_left.x, near_top_left.y, near_top_left.z);
    glVertex3f(far_top_left.x, far_top_left.y, far_top_left.z);

    glVertex3f(near_top_right.x, near_top_right.y, near_top_right.z);
    glVertex3f(far_top_right.x, far_top_right.y, far_top_right.z);

    glVertex3f(near_bottom_left.x, near_bottom_left.y, near_bottom_left.z);
    glVertex3f(far_bottom_left.x, far_bottom_left.y, far_bottom_left.z);

    glVertex3f(near_bottom_right.x, near_bottom_right.y, near_bottom_right.z);
    glVertex3f(far_bottom_right.x, far_bottom_right.y, far_bottom_right.z);

    glEnd();
}