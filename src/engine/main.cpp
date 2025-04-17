#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

#define _USE_MATH_DEFINES
#include <math.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/glut.h>
#endif

// PREPROCESSOR TO DISABLE VSYNC

#ifdef _WIN32
#include <windows.h>
typedef BOOL(WINAPI *PFNWGLSWAPINTERVALEXTPROC)(int);
#elif defined(__linux__)
#include <GL/glx.h>
typedef int (*PFNGLXSWAPINTERVALSGIPROC)(int);
#elif defined(__APPLE__)
#include <OpenGL/OpenGL.h>
#endif

#include "external/tinyxml2.h"

#include "engine/config.hpp"
#include "engine/group.hpp"
#include "engine/camera.hpp"
#include "engine/frustum.hpp"

#include "math/matrix4x4.hpp"
#include "math/vector3.hpp"
#include "math/vector4.hpp"

#include "utils/printer.hpp"

// important objects	----------------------------->>>

config *cfg_obj = NULL;
camera *cam = NULL;

matrix4x4 projection_matrix;
frustum view_frustum = frustum();

// < -------------------------------------------------

// window options	--------------------------------->>>

int win_width = 10, win_height = 10;

// projection stuff
float cam_fov, cam_near, cam_far;

// < -------------------------------------------------

// flags	----------------------------------------->>>

bool draw_axis = false;
bool wire_mode = false;
bool draw_path = false;
bool update_groups = true;

// frustum cull debug
bool draw_bounding_spheres = false;
bool draw_frustum = false;
bool frustum_cull = true;
bool update_frustum_on_free_cam = true;

// keep pressed key state for camera movement
bool key_states[256] = {false}; // array storing all keystates (if they're being held down)

// < -------------------------------------------------

// timers, clock times, fps, etc	----------------->>>
int timebase;
int prev_time;

int frames;
float fps;
// < -------------------------------------------------

void disable_vsync()
{
#ifdef _WIN32
	PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT =
		(PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");

	if (wglSwapIntervalEXT)
	{
		wglSwapIntervalEXT(0);
	}
#elif defined(__linux__)
	Display *dpy = glXGetCurrentDisplay();
	GLXDrawable drawable = glXGetCurrentDrawable();

	if (dpy && drawable)
	{
		PFNGLXSWAPINTERVALSGIPROC glXSwapIntervalSGI =
			(PFNGLXSWAPINTERVALSGIPROC)glXGetProcAddress((const GLubyte *)"glXSwapIntervalSGI");

		if (glXSwapIntervalSGI)
		{
			glXSwapIntervalSGI(0);
		}
	}
#elif defined(__APPLE__)
	CGLContextObj ctx = CGLGetCurrentContext();
	if (ctx)
	{
		GLint sync = 0;
		CGLSetParameter(ctx, kCGLCPSwapInterval, &sync);
	}
#endif
}

void change_window_size(int w, int h)
{
	if (h == 0)
		h = 1;

	cam->update_window_size(w, h);

	float ratio = w * 1.0 / h;

	projection_matrix = matrix4x4::Projection(cam_fov, ratio, cam_near, cam_far);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMultMatrixf(projection_matrix);
	glMatrixMode(GL_MODELVIEW);

	glViewport(0, 0, w, h);
}

void render_scene(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();

	// use cameras view matrix
	matrix4x4 view_matrix = cam->get_view_matrix();
	matrix4x4 projection_view = projection_matrix * view_matrix;

	glMultMatrixf(view_matrix);

#if defined(USE_LIGHTING)
	static float light_pos[4] = {0, 5.0f, 0, 1.0f};
	glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
#endif

	if (draw_axis)
	{
#ifdef USE_LIGHTING
		glDisable(GL_LIGHTING);
#endif

		glBegin(GL_LINES);
		// x axis
		glColor3f(1.0f, 0.0f, 0.0f);
		glVertex3f(-100.0f, 0.0f, 0.0f);
		glVertex3f(100.0f, 0.0f, 0.0f);

		// y axis
		glColor3f(0.0f, 1.0f, 0.0f);
		glVertex3f(0.0f, -100.0f, 0.0f);
		glVertex3f(0.0f, 100.0f, 0.0f);

		// z axis
		glColor3f(0.0f, 0.0f, 1.0f);
		glVertex3f(0.0f, 0.0f, -100.0f);
		glVertex3f(0.0f, 0.0f, 100.0f);
		glEnd();

#ifdef USE_LIGHTING
		glEnable(GL_LIGHTING);
#endif
	}
	glColor3f(1.0f, 1.0f, 1.0f);

	if (wire_mode)
		glPolygonMode(GL_FRONT, GL_LINE);
	else
		glPolygonMode(GL_FRONT, GL_FILL);

	// update frustum if camera is locked OR flag is set
	if (cam->update_frustum() || update_frustum_on_free_cam)
		view_frustum.update_frustum(projection_view);

	if (draw_frustum)
		view_frustum.draw_frustum();

	// render all meshes loaded in groups
	cfg_obj->render_all_groups(view_matrix, view_frustum, frustum_cull, draw_bounding_spheres, draw_path);

	frames++;
	int time = glutGet(GLUT_ELAPSED_TIME);
	if (time - timebase > 1000)
	{
		fps = (float)frames * 1000.0 / (float)(time - timebase);
		timebase = time;
		frames = 0;

		std::stringstream ss;
		ss << fps;

		glutSetWindowTitle(ss.str().data());
	}

	// End of frame
	glutSwapBuffers();
}

/**
 *
 * IDLE FUNCTION, IT IS VERY IMPORTANT TO UPDATE EVERYTHING EVERY FRAME!
 */
void idle()
{
	int current_time = glutGet(GLUT_ELAPSED_TIME);
	int delta_time_ms = current_time - prev_time;
	prev_time = current_time;

	if (update_groups)
	{
		cfg_obj->update_groups(delta_time_ms);
		std::vector<vector3> l_pos = cfg_obj->query_group_postitions();
		cam->update_lock_positions(l_pos);
	}

	cam->play_animations(delta_time_ms);
	cam->update_camera_position(key_states, delta_time_ms);
	glutPostRedisplay();
}

void processKeyPress(unsigned char c, int mouse_x, int mouse_y)
{
	switch (c)
	{
	case '1':
		draw_axis = !draw_axis;
		break;

	case '2':
		wire_mode = !wire_mode;
		break;

	case '3':
		draw_path = !draw_path;
		break;

	case '4':
		update_groups = !update_groups;
		break;

	case '5':
		draw_bounding_spheres = !draw_bounding_spheres;
		break;

	case '6':
		draw_frustum = !draw_frustum;
		break;

	case '7':
		frustum_cull = !frustum_cull;
		break;

	case '8':
		update_frustum_on_free_cam = !update_frustum_on_free_cam;
		break;

	case 'f':
	case 'F':
		cam->switch_camera_mode();
		break;

	case 'p':
	case 'P':
		printer::print_tutorial(glGetString(GL_VENDOR), glGetString(GL_RENDERER), glGetString(GL_VERSION));
		break;

	case 'r':
	case 'R':
		cam->reset_camera();
		break;

	case 'c':
	case 'C':
		cam->cycle_target();
		break;

	case 'z':
	case 'Z':
		cam->add_to_target_radius(-1.0f);
		break;

	case 'x':
	case 'X':
		cam->add_to_target_radius(1.0f);
		break;

	case 27:
		// this is esc!
		exit(0);
		break;

	default:
		// any key that has no special effect will instead be registered as being pressed down
		key_states[c] = true;
		break;
	}
}

void processKeyRelease(unsigned char c, int mouse_x, int mouse_y)
{
	// sets key state to being up (obvious, eh?)
	key_states[c] = false;
}

void processMouse(int x, int y)
{
	// x and y are the offset of the mouse from the previous frame
	cam->update_camera_direction(x, y);
}

int main(int argc, char **argv)
{
	printer::print_init();

	std::stringstream ss;
	if (argc != 2)
	{
		ss << "Wrong number of arguments!";
		printer::print_exception(ss.str(), "main");
		return 1;
	}

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(win_width, win_height);
	glutCreateWindow("Projeto CG-25");

	glutDisplayFunc(render_scene);
	glutIdleFunc(idle);
	glutReshapeFunc(change_window_size);

	glutKeyboardFunc(processKeyPress);
	glutKeyboardUpFunc(processKeyRelease);
	glutPassiveMotionFunc(processMouse);

#ifndef __APPLE__
	glewInit();
#endif

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glEnable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT, GL_LINE);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	glEnableClientState(GL_VERTEX_ARRAY);

#if defined(USE_LIGHTING)
	printer::print_info("USE_LIGTHING is defined...");

	glEnableClientState(GL_NORMAL_ARRAY);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	glEnable(GL_NORMALIZE);

	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE); // <- Phong

	float dark[4] = {0.2, 0.2, 0.2, 1.0};
	float white[4] = {1.0, 1.0, 1.0, 1.0};
	float black[4] = {0.0f, 0.0f, 0.0f, 0.0f};
	// light colors
	glLightfv(GL_LIGHT0, GL_AMBIENT, dark);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, white);
	glLightfv(GL_LIGHT0, GL_SPECULAR, white);
	// controls global ambient light
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, black);
#endif

	try
	{
		cfg_obj = new config(argv[1]);
	}
	catch (const FailedToLoadException &)
	{
		return 1;
	}
	catch (const FailedToParseGroupException &)
	{
		return 1;
	}
	catch (const std::exception &exc)
	{
		printer::print_exception(exc.what(), "unknown");
		return 1;
	}

	// get camera from config
	cam = cfg_obj->get_config_camera_init();

	// window attributes are stored in cfg
	std::tuple<int, int> win_attribs = cfg_obj->get_window_attributes();
	int width = std::get<0>(win_attribs), height = std::get<1>(win_attribs);
	glutReshapeWindow(width, height);

	vector3 projection_attributes = cfg_obj->get_projection_settings();
	cam_fov = projection_attributes.x;
	cam_near = projection_attributes.y;
	cam_far = projection_attributes.z;

	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

	timebase = glutGet(GLUT_ELAPSED_TIME);
	prev_time = glutGet(GLUT_ELAPSED_TIME);

	disable_vsync();

	// set view frustum before first frame!
	if (height == 0)
		height = 1;
	cam->update_window_size(width, height);
	float ratio = width * 1.0 / height;

	projection_matrix = matrix4x4::Projection(cam_fov, ratio, cam_near, cam_far);
	matrix4x4 proj_view = projection_matrix * cam->get_view_matrix();
	view_frustum.update_frustum(proj_view);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMultMatrixf(projection_matrix);
	glMatrixMode(GL_MODELVIEW);

	// enable ansi code support on windows
#ifdef _WIN32
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	DWORD dwMode = 0;
	GetConsoleMode(hOut, &dwMode);
	dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
	SetConsoleMode(hOut, dwMode);
#endif

	// printer::print_exception("This is a test.");
	// printer::print_warning("This is a test.");

	// MAIN LOOP!!!!!
	glutMainLoop();

	return 1;
}
