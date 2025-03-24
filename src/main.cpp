#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#define _USE_MATH_DEFINES
#include <math.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/glut.h>
#endif


//PREPROCESSOR TO DISABLE VSYNC

#ifdef _WIN32
    #include <windows.h>
    typedef BOOL(WINAPI *PFNWGLSWAPINTERVALEXTPROC)(int);
#elif defined(__linux__)
    #include <GL/glx.h>
    typedef int (*PFNGLXSWAPINTERVALSGIPROC)(int);
#elif defined(__APPLE__)
    #include <OpenGL/OpenGL.h>
#endif

#include <vector>

#include "tinyxml2.h"
#include "config.hpp"
#include "group.hpp"
#include "matrix4x4.hpp"
#include "vector3.hpp"


config *cfg_obj = NULL;
camera *cam = NULL;

matrix4x4 projection_matrix;
frustum *view_frustum = NULL;

//window options
int win_width = 10, win_height = 10;
float cam_fov, cam_near, cam_far;

//flag
bool draw_axis = true;
bool wire_mode = true;
bool draw_bounding_spheres = false;
bool draw_frustum = false;
bool frustum_cull = true;

bool key_states[256] = {false}; //array storing all keystates (if theyre being held down)

//timers, clock times, fps, etc
int timebase;
int prev_time;

int frames;
float fps;



void disable_vsync() {
	#ifdef _WIN32
		PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT =
			(PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");
	
		if (wglSwapIntervalEXT) {
			wglSwapIntervalEXT(0); // Disable VSync
		}
	#elif defined(__linux__)
		Display* dpy = glXGetCurrentDisplay();
		GLXDrawable drawable = glXGetCurrentDrawable();
	
		if (dpy && drawable) {
			PFNGLXSWAPINTERVALSGIPROC glXSwapIntervalSGI =
				(PFNGLXSWAPINTERVALSGIPROC)glXGetProcAddress((const GLubyte*)"glXSwapIntervalSGI");
	
			if (glXSwapIntervalSGI) {
				glXSwapIntervalSGI(0); // Disable VSync
			}
		}
	#elif defined(__APPLE__)
		CGLContextObj ctx = CGLGetCurrentContext();
		if (ctx) {
			GLint sync = 0;
			CGLSetParameter(ctx, kCGLCPSwapInterval, &sync); // Disable VSync
		}
	#endif
}

void change_window_size(int w, int h) {
	if(h == 0)
		h = 1;

	cam->update_window_size(w, h);

	float ratio = w * 1.0 / h;

	projection_matrix = matrix4x4::Projection(cam_fov, ratio, cam_near, cam_far);

    glViewport(0, 0, w, h);

	glMatrixMode(GL_PROJECTION);

	glLoadIdentity();
	//glMultMatrixf(projection_matrix.get_data());

	glMatrixMode(GL_MODELVIEW);
}


void render_scene(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	//use cameras view matrix
	matrix4x4 view_matrix = cam->get_view_matrix();

	matrix4x4 projection_view = projection_matrix * view_matrix;
	
	glMultMatrixf(projection_view.get_data());
	//glMultMatrixf(view_matrix.get_data());

	if(draw_axis) {
		glBegin(GL_LINES);
			//x axis
			glColor3f(1.0f, 0.0f, 0.0f);
			glVertex3f(-100.0f, 0.0f, 0.0f);
			glVertex3f( 100.0f, 0.0f, 0.0f);

			//y axis
			glColor3f(0.0f, 1.0f, 0.0f);
			glVertex3f(0.0f, -100.0f, 0.0f);
			glVertex3f(0.0f,  100.0f, 0.0f);

			//z axis
			glColor3f(0.0f, 0.0f, 1.0f);
			glVertex3f(0.0f, 0.0f, -100.0f);
			glVertex3f(0.0f, 0.0f,  100.0f);
		glEnd();
	}
	glColor3f(1.0f, 1.0f, 1.0f);

	if(wire_mode) {
		glPolygonMode(GL_FRONT, GL_LINE);
	} 
	else {
		glPolygonMode(GL_FRONT, GL_FILL);
	}


	if(cam->update_frustum())
		view_frustum = new frustum(projection_view);

	if(draw_frustum)
		view_frustum->draw_frustum();


	//render all meshes loaded in groups

	cfg_obj->render_all_groups(*view_frustum, frustum_cull, draw_bounding_spheres);
	
	
	//fps counter
	char str[20];

	frames++;
	int time = glutGet(GLUT_ELAPSED_TIME);
	if (time - timebase > 1000) {
		fps = (float)frames*1000.0/(float)(time-timebase);
		timebase = time;
		frames = 0;
		
		sprintf_s(str, "%.4f", fps);

		glutSetWindowTitle(str);
	}
	

	// End of frame
	glutSwapBuffers();
}

void idle() {
	int current_time = glutGet(GLUT_ELAPSED_TIME);
	int delta_time_ms = current_time - prev_time;
	prev_time = current_time;

	cam->play_animations(delta_time_ms);
	cam->update_camera_position(key_states, delta_time_ms);
	glutPostRedisplay();
}

void processKeyPress(unsigned char c, int mouse_x, int mouse_y) {
	switch(c) {
		case '1':
			draw_axis = !draw_axis;
			break;

		case '2':
			wire_mode = !wire_mode;
			break;

		case '3':
			draw_bounding_spheres = !draw_bounding_spheres;
			break;

		case '4':
			draw_frustum = !draw_frustum;
			break;

		case '5':
			frustum_cull = !frustum_cull;
			break;

		case 'f':
		case 'F':
			cam->switch_camera_mode();
			break;

		/*
		case 'p':
		case 'P':
			//was used to print info, now unused
			break;
		*/


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
			exit(0);
			break;

		default:
			key_states[c] = true;
			break;
	}
}

void processKeyRelease(unsigned char c, int mouse_x, int mouse_y) {
	if(false) {
		//skip for now
	}
	else {
		key_states[c] = false;
	}
}

void processMouse(int x, int y) {
	cam->update_camera_direction(x, y);
}

void processSpecialKeys(int key, int xx, int yy) {

}


void printInfo() {

	std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
	std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
	std::cout << "Version: " << glGetString(GL_VERSION) << "\n" << std::endl;

	std::cout << "Press 1 to toggle axis rendering." << std::endl;
	std::cout << "Press 2 to toggle between wire and solid rendering mode." << std::endl;
	std::cout << "Press f/F to switch between fixed camera and free camera mode." << std::endl;
	std::cout << "Press c/C to change camera target (in fixed mode)." << std::endl;
	std::cout << "Press r/R to reset camera to first target." << std::endl;
}


int main(int argc, char **argv) {
	if(argc != 2) {
		std::cout << "Wrong number of arguments!" << std::endl;
		return 1;
	}

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH|GLUT_DOUBLE|GLUT_RGBA);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(win_width,win_height);
	glutCreateWindow("Projeto CG-25");
		
	glutDisplayFunc(render_scene);
	glutIdleFunc(idle);
	glutReshapeFunc(change_window_size);
	
	glutKeyboardFunc(processKeyPress);
	glutKeyboardUpFunc(processKeyRelease);
	glutPassiveMotionFunc(processMouse);
	glutSpecialFunc(processSpecialKeys);
	
	
	#ifndef __APPLE__
	glewInit();
	#endif
	
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	glPolygonMode(GL_FRONT, GL_LINE);
	
	glEnableClientState(GL_VERTEX_ARRAY);
	
	printInfo();
	
	cfg_obj = new config(argv[1]);

	cam = cfg_obj->get_config_camera_init();

	std::tuple<int, int> win_attribs = cfg_obj->get_window_attributes();
	glutReshapeWindow(std::get<0>(win_attribs), std::get<1>(win_attribs));
	
	vector3 projection_attributes = cfg_obj->get_projection_settings();
	cam_fov = projection_attributes.x;
	cam_near = projection_attributes.y;
	cam_far = projection_attributes.z;

	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

	timebase = glutGet(GLUT_ELAPSED_TIME);
	prev_time = glutGet(GLUT_ELAPSED_TIME);

	disable_vsync();

	//this means using a custom projection and view matrix!
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);

	glutMainLoop();
	
	return 1;
}
