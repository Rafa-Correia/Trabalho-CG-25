

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


config *cfg_obj = NULL;
camera *cam = NULL;

//window options
int win_width = 10, win_height = 10;
float cam_fov, cam_near, cam_far;

//locked cam
float radius, alpha, beta;
float cam_x, cam_y, cam_z;
float cam_lookat_x, cam_lookat_y, cam_lookat_z;
float cam_up_x, cam_up_y, cam_up_z;

//flag
bool in_free_cam = false;
bool draw_axis = true;
bool wire_mode = true;

bool key_states[256] = {false}; //array storing all keystates (if theyre being held down)

//timers, clock times, fps, etc
int prevTime;

int frames;
float fps;



void disableVSync() {
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


void spherical2Cartesian() {
	cam_x = radius * cos(beta) * sin(alpha);
	cam_y = radius * sin(beta);
	cam_z = radius * cos(beta) * cos(alpha);
}

void cartesian2Spherical() {
    radius = std::sqrt(cam_x * cam_x + cam_y * cam_y + cam_z * cam_z);
    beta = std::asin(cam_y / radius);
    alpha = std::atan2(cam_x, cam_z);
}


void changeSize(int w, int h) {

	// Prevent a divide by zero, when window is too short
	// (you cant make a window with zero width).

	if(h == 0)
		h = 1;

	cam->update_window_size(w, h);

	// compute window's aspect ratio 
	float ratio = w * 1.0 / h;


	// Set the projection matrix as current
	glMatrixMode(GL_PROJECTION);
	// Load Identity Matrix
	glLoadIdentity();
	
	// Set the viewport to be the entire window
    glViewport(0, 0, w, h);

	// Set perspective
	gluPerspective(cam_fov ,ratio, cam_near ,cam_far);

	// return to the model view matrix mode
	glMatrixMode(GL_MODELVIEW);
}


void renderScene(void) {

	// clear buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//use cameras view matrix
	if(in_free_cam) {
		cam->camera_glu_lookat();
	}
	else {
		glLoadIdentity();
		gluLookAt(	cam_x, cam_y, cam_z, 
					cam_lookat_x, cam_lookat_y, cam_lookat_z,
					cam_up_x, cam_up_y, cam_up_z);

		std::cout 	<< cam_x << " " << cam_y << " " << cam_z << "\n"
					<< cam_lookat_x << " " << cam_lookat_y << " " << cam_lookat_z << "\n"
					<< cam_up_x << " " << cam_up_y << " " << cam_up_z << std::endl;			
		std::cout << "Loaded locked camera view matrix!" << std::endl;
	}
	
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

	cfg_obj->render_all_groups();
	

	/*
	//fps counter
	char str[20];

	frames++;
	int time = glutGet(GLUT_ELAPSED_TIME);
	if (time - timebase > 1000) {
		//std::cout << time << "|" << timebase << "|" << frames << std::endl;
		fps = (float)frames*1000.0/(float)(time-timebase);
		timebase = time;
		frames = 0;

		//std::cout << fps << std::endl;
		
		sprintf(str, "%.4f", fps);

		//std::cout << "str: " << str << std::endl;

		glutSetWindowTitle(str);
	}
	*/

	// End of frame
	glutSwapBuffers();
}

void idle() {
	//process camera movement here
	int current_time = glutGet(GLUT_ELAPSED_TIME);
	int delta_time = prevTime - current_time;
	prevTime = current_time;

	if(in_free_cam) {
		cam->update_camera_position(key_states, delta_time);
		glutPostRedisplay();
	}	

}

void processKeyDown(unsigned char c, int mouse_x, int mouse_y) {
	if(c == '1') {
		draw_axis = !draw_axis;
	}
	else if(c == '2') {
		wire_mode = !wire_mode;
	}
	else if(c == 'f' || c == 'F') {
		in_free_cam = !in_free_cam;
		if(!in_free_cam) {
			glutSetCursor(GLUT_CURSOR_INHERIT);
		}
		else {
			glutSetCursor(GLUT_CURSOR_NONE);
			cam->just_switched();
		}
		glutPostRedisplay();
	}
	else if(c == 27) { //esc
		exit(0);
	}
	else {
		key_states[c] = true;
	}
}

void processKeyUp(unsigned char c, int mouse_x, int mouse_y) {
	if(false) {
		//skip for now
	}
	else {
		key_states[c] = false;
	}
}

void processMouse(int x, int y) {
	if(in_free_cam)
		cam->update_camera_direction(x, y);
}

void processSpecialKeys(int key, int xx, int yy) {
	if(in_free_cam) return;
	switch (key) {
		case GLUT_KEY_RIGHT:
			alpha -= 0.1; 
			break;
		case GLUT_KEY_LEFT:
			alpha += 0.1;
			break;
		case GLUT_KEY_UP:
			beta += 0.1f;
			if (beta > 1.5f)
				beta = 1.5f;
			break;
		case GLUT_KEY_DOWN:
			beta -= 0.1f;
			if (beta < -1.5f)
				beta = -1.5f;
			break;
		case GLUT_KEY_PAGE_DOWN: radius -= 1.0f;
			if (radius < 0.1f)
				radius = 0.1f;
			break;
		case GLUT_KEY_PAGE_UP: 
			radius += 1.0f;
			break;
	}
	spherical2Cartesian();
	glutPostRedisplay();
}


void printInfo() {

	printf("Vendor: %s\n", glGetString(GL_VENDOR));
	printf("Renderer: %s\n", glGetString(GL_RENDERER));
	printf("Version: %s\n\n", glGetString(GL_VERSION));
}


int main(int argc, char **argv) {
	
	srand((unsigned int)time(NULL)); // seed rng

// init GLUT and the window
	if(argc != 2) {
		std::cout << "Wrong number of arguments!" << std::endl;
		return 1;
	}

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH|GLUT_DOUBLE|GLUT_RGBA);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(win_width,win_height);
	glutCreateWindow("Projeto CG-25");
		
// Required callback registry 
	glutDisplayFunc(renderScene);
	glutIdleFunc(idle);
	glutReshapeFunc(changeSize);
	
	// Callback registration for keyboard processing
	glutKeyboardFunc(processKeyDown);
	glutKeyboardUpFunc(processKeyUp);
	glutPassiveMotionFunc(processMouse);

	glutSpecialFunc(processSpecialKeys);
	
	// init GLEW
	#ifndef __APPLE__
	glewInit();
	#endif
	
	//  OpenGL settings
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT, GL_LINE);
	
	glEnableClientState(GL_VERTEX_ARRAY);

	//glutSetCursor(GLUT_CURSOR_NONE);
	//prepare_data();
	
	
	//readfile(1);
	/*
	std::cout << "Loading config file..." << std::endl; 
	if(!load_config_file(std::string(argv[1]))) return 1;
	std::cout << "Done!\n" << std::endl;
	*/
	
	printInfo();
	
	cfg_obj = new config(argv[1]);
	cfg_obj->print_info();

	cam = cfg_obj->get_config_camera_init();

	cfg_obj->prepare_all_groups();

	std::vector<group> root_groups = cfg_obj->get_root_groups();
	group *root_groups_data = root_groups.data();

	for(size_t i = 0; i < root_groups.size(); i++) {
		group g = root_groups_data[i];
		g.print_group("|");
	}


	std::tuple<int, int> win_attribs = cfg_obj->get_window_attributes();
	glutReshapeWindow(std::get<0>(win_attribs), std::get<1>(win_attribs));
	
	std::tuple<float, float, float> projection_attributes = cfg_obj->get_projection_settings();
	cam_fov = std::get<0>(projection_attributes);
	cam_near = std::get<1>(projection_attributes);
	cam_far = std::get<2>(projection_attributes);

	std::tuple<float, float, float> locked_cam_position = cfg_obj->get_locked_cam_pos();
	std::tuple<float, float, float> locked_cam_lookat = cfg_obj->get_locked_cam_lookat();
	std::tuple<float, float, float> locked_cam_up = cfg_obj->get_locked_cam_up();

	cam_x = std::get<0>(locked_cam_position);
	cam_y = std::get<1>(locked_cam_position);
	cam_z = std::get<2>(locked_cam_position);

	cam_lookat_x = std::get<0>(locked_cam_lookat);
	cam_lookat_y = std::get<1>(locked_cam_lookat);
	cam_lookat_z = std::get<2>(locked_cam_lookat);

	cam_up_x = std::get<0>(locked_cam_up);
	cam_up_y = std::get<1>(locked_cam_up);
	cam_up_z = std::get<2>(locked_cam_up);

	cartesian2Spherical();
	
	//config c_loaded(argv[1]);
	    

	

	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

	prevTime = glutGet(GLUT_ELAPSED_TIME);

	disableVSync();

// enter GLUT's main cycle
	glutMainLoop();
	
	return 1;
}
