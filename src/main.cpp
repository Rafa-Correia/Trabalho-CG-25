

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

#include <vector>

#include "tinyxml2.h"
#include "config.hpp"
#include "group.hpp"


config *cfg_obj = NULL;

//window options
int win_width = 10, win_height = 10;

//camera options
float alfa, beta, radius;

float cam_x, cam_y, cam_z;
float cam_lookat_x, cam_lookat_y, cam_lookat_z;
float up_x, up_y, up_z;
float cam_fov, cam_near, cam_far;

//GLuint vertices, verticeCount, indices, indexCount;

std::vector<GLuint> vertex_buffers, index_buffers;
std::vector<std::size_t> index_count;
unsigned long long int current_buffer = 0;

void spherical2Cartesian() {
	cam_x = radius * cos(beta) * sin(alfa);
	cam_y = radius * sin(beta);
	cam_z = radius * cos(beta) * cos(alfa);
}

void cartesian2Spherical() {
    radius = std::sqrt(cam_x * cam_x + cam_y * cam_y + cam_z * cam_z);
    beta = std::asin(cam_y / radius);
    alfa = std::atan2(cam_x, cam_z);
}

void changeSize(int w, int h) {

	// Prevent a divide by zero, when window is too short
	// (you cant make a window with zero width).
	if(h == 0)
		h = 1;

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

	// set the camera
	glLoadIdentity();
	gluLookAt(cam_x, cam_y, cam_z,
		cam_lookat_x, cam_lookat_y, cam_lookat_z,
		up_x, up_y, up_z);

	//change here to draw all loaded models
	//glBindBuffer(GL_ARRAY_BUFFER, vertices);
	//glVertexPointer(3, GL_FLOAT, 0, 0);

	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices);
	//glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
	cfg_obj->render_all_groups();
	

	// End of frame
	glutSwapBuffers();
}

void processKeys(unsigned char c, int xx, int yy) {

// put code to process regular keys in here

}

void processSpecialKeys(int key, int xx, int yy) {

	switch (key) {

	case GLUT_KEY_RIGHT:
		alfa -= 0.1; break;

	case GLUT_KEY_LEFT:
		alfa += 0.1; break;

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

	case GLUT_KEY_PAGE_UP: radius += 1.0f; break;
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
	glutReshapeFunc(changeSize);
	//glutIdleFunc(renderScene);
	
	// Callback registration for keyboard processing
	glutKeyboardFunc(processKeys);
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

	cfg_obj->prepare_all_groups();

	std::vector<group> root_groups = cfg_obj->get_root_groups();
	group *root_groups_data = root_groups.data();
	for(size_t i = 0; i < root_groups.size(); i++) {
		group g = root_groups_data[i];
		g.print_group("|");
	}


	std::tuple<int, int> win_attribs = cfg_obj->get_window_attributes();
	glutReshapeWindow(std::get<0>(win_attribs), std::get<1>(win_attribs));


	std::tuple<float, float, float> camera_position = cfg_obj->get_camera_position();
	cam_x = std::get<0>(camera_position);
	cam_y = std::get<1>(camera_position);
	cam_z = std::get<2>(camera_position);

	std::tuple<float, float, float> camera_lookAt = cfg_obj->get_camera_lookAt();
	cam_lookat_x = std::get<0>(camera_lookAt);
	cam_lookat_y = std::get<1>(camera_lookAt);
	cam_lookat_z = std::get<2>(camera_lookAt);
	
	std::tuple<float, float, float> camera_up_vector = cfg_obj->get_camera_up_vector();
	up_x = std::get<0>(camera_up_vector);
	up_y = std::get<1>(camera_up_vector);
	up_z = std::get<2>(camera_up_vector);
	
	std::tuple<float, float, float> projection_attributes = cfg_obj->get_projection_settings();
	cam_fov = std::get<0>(projection_attributes);
	cam_near = std::get<1>(projection_attributes);
	cam_far = std::get<2>(projection_attributes);


	cartesian2Spherical();
	

	
	//config c_loaded(argv[1]);
	

	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

// enter GLUT's main cycle
	glutMainLoop();
	
	return 1;
}
