#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "matrix4x4.hpp"
#include "vector3.hpp"

#include <iostream>
#include <vector>

#define _USE_MATH_DEFINES
#include <math.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/glut.h>
#endif

// these are purelly identifiers

#define C_ANIMATION_IDLE 0
#define C_ANIMATION_CAMERA_LOCKING 1
#define C_ANIMATION_CHANGE_TARGET 2
#define C_ANIMATION_CHANGE_RADIUS 3

#define C_DURATION_CAMERA_LOCKING 500 // < -- duration in ms
#define C_DURATION_CHANGE_TARGET 750  // < -- duration in ms
#define C_DURATION_CHANGE_RADIUS 50   // < -- duration in ms

class camera
{
public:
    camera();
    camera(vector3 pos, vector3 lock_point, vector3 up, std::vector<vector3> l_pos = std::vector<vector3>());

    /**
     * Changes camera position based on what keys are pressed. Moves at constant rate, independant on framerate.
     *
     * @param key_states Array of booleans keeping track of what keys are being held down.
     * @param delta_time_ms Time since last update in miliseconds.
     */
    void update_camera_position(const bool key_states[256], int delta_time_ms);

    /**
     * Changes direction camera is facing based on mouse movement. Won't change anything in fixed camera mode.
     *
     * @param x x position of mouse cursor relative to upper right corner.
     * @param y y position of mouse cursor relative to upper right corner.
     */
    void update_camera_direction(int x, int y);

    /**
     * Updates internal state keeping track of window width and height.
     *
     * @param width New width of window.
     * @param height New height of window.
     */
    void update_window_size(int width, int height);

    /**
     * Camera sets view matrix based on it's parameters.
     */
    matrix4x4 get_view_matrix();

    /**
     * Switches between fixed camera mode and free camera mode. Small easing animation will play on transition from free camera mode to fixed camera mode, taking control away for 0.5 sec.
     */
    void switch_camera_mode();

    /**
     * Debug function that prints camera's parameters.
     */
    void print_info();

    /**
     * Plays transition animation from free to fixed camera mode.
     *
     * @param delta_time_ms Time since last update in ms.
     */
    void play_animations(int delta_time_ms);

    /**
     * Resets camera to some default state.
     */
    void reset_camera();

    /**
     * Cycles camera target.
     */
    void cycle_target();

    /**
     * Adds given delta to camera target radius.
     *
     * @param delta Amount to change target radius by.
     */
    void add_to_target_radius(float delta);

    /**
     * Determines if view frustum needs to be updated.
     *
     * @returns Boolean determining if frustum needs updating.
     */
    bool update_frustum();

    /**
     *
     */
    void update_lock_positions(std::vector<vector3> &l_pos);

private:
    vector3 pos; // < -- position of the camera
    vector3 up;  // < -- camera's "up" vector (vector pointing up)
    vector3 dir; // < -- camera direction, used in free cam mode

    vector3 lock_point; // < -- camera always looks at this point in fixed mode

    vector3 target_lock_point; // < -- camera's lock point (in fixed mode, after animation, target point will be equal to this)
    vector3 start_lock_point;  // < -- when playing lerp camera transition, this serves as the start point for camera target

    vector3 target_pos; // < -- camera's target position (will move to this point during change target animation)
    vector3 start_pos;  // < -- serves as starting posititon in lerp transition of movement

    float target_radius; // < -- camera's target radius
    float start_radius;  // < -- serves as starting radius when playing change radius animation

    float radius, alpha, beta;      // < -- when camera is locked, use spherical coordinates.
    float yaw = 0.0f, pitch = 0.0f; // < -- when in free camera mode use yaw and pitch do turn camera with mouse.

    int center_x = 0, center_y = 0; // < -- screen center for locking cursor in free camera mode

    int current_animation = C_ANIMATION_IDLE; // < -- current animation, if no animation being played, then it is idle

    int animation_timer = 0; // < -- timer for current animation

    float linear_movement_speed = 50.0f; // < -- movement speed of camera in free camera mode. In units / sec.
    float angular_movement_speed = 1.0f; // < -- angular speed of camera in locked camera mode. In radians / sec.
    float zoom_in_speed = 5.0f;          // < -- radius change speed in locked camera mode. In units / sec.
    float mouse_sensitivity = 0.06f;     // < -- mouse sensitivity. I do not know the units :)

    bool is_free_camera = false;   // < -- state flag. If true then using free camera mode, otherwise using locked camera mode.
    bool just_warped = false;      // < -- state flag. Indicates if mouse cursor was warped to center last update, since glutWarpPointer triggers another update event.
    bool animation_locked = false; // < -- state flag. Indicates if animation is currently being played.

    unsigned int current_target = 0;      // < -- current camera target.
    std::vector<vector3> lock_point_list; // < -- list of points for camera locking.

    /**
     * Updates spherical coordinates based on the cartesian coordinates.
     */
    void cartesian_to_spherical_coords(bool set_target_radius = false);

    /**
     * Updates cartesian coordinates based on the spherical coordinates.
     */
    void spherical_to_cartesian_coords(bool to_target = false);

    /**
     * Calculates angle value from radians to degrees.
     *
     * @param ang_r Angle in radians.
     *
     * @returns Angle in degrees.
     */
    float radian_to_degree(float ang_r);

    /**
     * Function responsible for setting animation and preparing all parameters.
     *
     * @param animation Animation to be set (use defines).
     */
    void set_animation(int animation);

    /**
     * Function responsible for animating camera locking.
     *
     * @param delta_time_ms Time since last update in ms.
     */
    void animate_camera_locking(int delta_time_ms);

    /**
     * Function responsible for animating target transition.
     *
     * @param delta_time_ms Time since last update in ms.
     */
    void animate_changing_target(int delta_time_ms);

    /**
     * Function responsible for animating radius change.
     *
     * @param delta_time_ms Time since last update in ms.
     */
    void animate_radius_change(int delta_time_ms);
};

#endif