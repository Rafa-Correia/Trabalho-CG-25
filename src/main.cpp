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
	GLuint *vertex_buffer_array = vertex_buffers.data();
	GLuint *index_buffer_array = index_buffers.data();
	std::size_t *index_count_array = index_count.data();
	for(unsigned long long int i = 0; i < current_buffer; i++) {
		GLuint vertices = vertex_buffer_array[i];
		GLuint indices = index_buffer_array[i];
		std::size_t n_indices = index_count_array[i];


		glBindBuffer(GL_ARRAY_BUFFER, vertices);
		glVertexPointer(3, GL_FLOAT, 0, 0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices);
		glDrawElements(GL_TRIANGLES, n_indices, GL_UNSIGNED_INT, 0);
	}
	

	// End of frame
	glutSwapBuffers();
}


void processKeys(unsigned char c, int xx, int yy) {

// put code to process regular keys in here

}

//create a read from file function here?
bool readfile(int vbo_index, const char *filepath) {
	std::cout << "Trying to load file: " << filepath << std::endl;

	std::vector<float> p;
	std::vector<unsigned int> i;

	std::ifstream file(filepath); // Open file RAFA

    if (!file) {
        std::cerr << "Error opening file\n";
        return false;
    }

    std::string line;
	int line_index = 0;
    while (std::getline(file, line)) { // Read file line by line
        std::stringstream ss(line);  // Use stringstream to parse the line
        std::string token;

        // Split the line by ';'
		if (line_index == 0) {
			//skipped for now, settings line
			//assumed all files have indices and no normals or tex coords
		}
		if (line_index == 1) {
			//indices
			while (std::getline(ss, token, ';')) {
				//std::cout << "i: " << line_index << " | " << "Parsed token: " << token << '\n'; // Output each token
				int index = std::stoi(token);
				i.push_back(index);
			}
		}
		if(line_index == 2) {
			//vertices
			while (std::getline(ss, token, ';')) {
            	//std::cout << "i: " << line_index << " | " << "Parsed token: " << token << '\n'; // Output each token
				float vertex_float = std::stof(token);
				p.push_back(vertex_float);
        	}
		}
        line_index++;
    }

    file.close();
	
	current_buffer++;

	std::cout << "Finished reading file, now creating VBO\\EBO (" << current_buffer << ")..." << std::endl;

	GLuint vertices, indices;

	GLenum err;
	while ((err = glGetError()) != GL_NO_ERROR) {
    	std::cerr << "OpenGL Error: " << err << std::endl;
	}


	//generate VBO
	glGenBuffers(1, &vertices);

	//std::cout << vertices << std::endl;

	glBindBuffer(GL_ARRAY_BUFFER, vertices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * p.size(), p.data(), GL_STATIC_DRAW);

	std::cout << "Finished generating VBO..." << std::endl;

	//EBO
	glGenBuffers(1, &indices);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
					sizeof(unsigned int) * i.size(),
				i.data(),
				GL_STATIC_DRAW);

	vertex_buffers.push_back(vertices);
	index_buffers.push_back(indices);
	//vertex_count.push_back(p.size() / 3);
	index_count.push_back(i.size());

	std::cout << "Finished creating VBO\\EBO!" << std::endl;

	return true;
}

bool load_config_file(std::string filepath) {
	tinyxml2::XMLDocument doc;
	if(doc.LoadFile(filepath.data()) != tinyxml2::XML_SUCCESS) {
		std::cout << "Failed to load XML config file!" << std::endl;
		return false;
	}

	tinyxml2::XMLElement *root = doc.RootElement();
	if(root) {
		if(std::string(root->Value()) != "world") {
			std::cout << "Unexpected root element." << std::endl;
			return false;
		}

		tinyxml2::XMLElement *window = root->FirstChildElement("window");
		if(window) {
			int width = -1, height = -1;
			window->QueryIntAttribute("width", &width);
			window->QueryIntAttribute("height", &height);

			if(width == -1 || height == -1) { //check if loaded
				std::cout << "There was a problem loading width\\height!" << std::endl;
				return false;
			}
			if(width <= 0 || height <= 0) { //check if valid
				std::cout << "Width\\Height is not valid!" << std::endl;
				return false;
			}

			win_width = width;
			win_height = height;

			std::cout << "Width: " << width << std::endl;
			std::cout << "Height: " << height << std::endl;
		} else {
			std::cout << "No window element!" << std::endl;
			return false;
		}

		tinyxml2::XMLElement *camera = root->FirstChildElement("camera");
		if(camera) {
			tinyxml2::XMLElement *position = camera->FirstChildElement("position");
			if(position) {
				float x = 0, y = 0, z = 0;
				position->QueryFloatAttribute("x", &x);
				position->QueryFloatAttribute("y", &y);
				position->QueryFloatAttribute("z", &z);

				cam_x = x;
				cam_y = y;
				cam_z = z;
			} else {
				std::cout << "No position element!" << std::endl;
				return false;
			}

			tinyxml2::XMLElement *lookat = camera->FirstChildElement("lookAt");
			if(lookat) {
				float x = 0, y = 0, z = 0;
				lookat->QueryFloatAttribute("x", &x);
				lookat->QueryFloatAttribute("y", &y);
				lookat->QueryFloatAttribute("z", &z);

				cam_lookat_x = x;
				cam_lookat_y = y;
				cam_lookat_z = z;
			} else {
				std::cout << "No lookAt element!" << std::endl;
				return false;
			}

			tinyxml2::XMLElement *up = camera->FirstChildElement("up");
			if(up) {
				float x = 0, y = 0, z = 0;
				up->QueryFloatAttribute("x", &x);
				up->QueryFloatAttribute("y", &y);
				up->QueryFloatAttribute("z", &z);

				up_x = x;
				up_y = y;
				up_z = z;
			} else {
				std::cout << "No up element!" << std::endl;
				return false;
			}

			
			tinyxml2::XMLElement *projection = camera->FirstChildElement("projection");
			if(projection) {
				float fov = -1, near = -1, far = -1;
				projection->QueryFloatAttribute("fov", &fov);
				projection->QueryFloatAttribute("near", &near);
				projection->QueryFloatAttribute("far", &far);

				if(fov == -1 || near == -1 || far == -1) {
					std::cout << "Camera fov\\near plane\\far plane wasn't loaded!" << std::endl;
					return false;
				}
				if(fov <= 0 || near <= 0 || far <= 0) {
					std::cout << "Camera fov\\near plane\\far plane isn't valid!" << std::endl;
					return false;
				}

				cam_fov = fov;
				cam_near = near;
				cam_far = far;
			} else {
				std::cout << "No projection element!" << std::endl;
				return false;
			}

		} else {
			std::cout << "No camera element!" << std::endl;
			return false;
		}

		bool loaded_group_at_least_once = false;
		tinyxml2::XMLElement *group = root->FirstChildElement("group");
		while(group) {
			loaded_group_at_least_once = true;
			//load transforms here

			//CHANGE THIS WHEN LOADING TRANSFORMS
			//FIRSTCHILDELEMENT -> NEXTSIBLINGELEMENT
			tinyxml2::XMLElement *models = group->FirstChildElement("models");
			if(models) {
				bool loaded_model_at_least_once = false;
				tinyxml2::XMLElement *model = models->FirstChildElement("model");
				while(model) {
					loaded_model_at_least_once = true;
					const char* filepath = model->Attribute("file");
					if(filepath) {
						if(!readfile(1, filepath)) {
							std::cout << "file attribute is invalid!" << std::endl;
							return false;
						}
					} else {
						std::cout << "A model element must have a file attribute!" << std::endl;
						return false;
					}
					model = model->NextSiblingElement("model");
				}
				if(!loaded_model_at_least_once) {
					std::cout << "A models element must have at least one model child element!" << std::endl;
					return false;
				}
			} else {
				std::cout << "A group element must have models child element!" << std::endl;
				return false;
			}
			group = group->NextSiblingElement("group");
		}

		if(!loaded_group_at_least_once) {
			std::cout << "At least one group element is mandatory!" << std::endl;
			return false;
		}

		return true;
	} else {
		std::cout << "Failed to load root element!" << std::endl;
		return false;
	}
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

	case GLUT_KEY_PAGE_DOWN: radius -= 0.1f;
		if (radius < 0.1f)
			radius = 0.1f;
		break;

	case GLUT_KEY_PAGE_UP: radius += 0.1f; break;
	}
	spherical2Cartesian();

	glutPostRedisplay();

}


void printInfo() {

	printf("Vendor: %s\n", glGetString(GL_VENDOR));
	printf("Renderer: %s\n", glGetString(GL_RENDERER));
	printf("Version: %s\n", glGetString(GL_VERSION));

	//printf("\nUse Arrows to move the camera up/down and left/right\n");
	//printf("Page Up and Page Down control the distance from the camera to the origin\n");
	std::cout << "Init camera pos -> x: " << cam_x << ", y: " << cam_y << ", z: " << cam_z << std::endl;
	std::cout << "Init camera lookAt -> x: " << cam_lookat_x << ", y: " << cam_lookat_y << ", z: " << cam_lookat_z << std::endl;
	std::cout << "Init camera up -> x: " << up_x << ", y: " << up_y << ", z: " << up_z << std::endl;
	std::cout << "A/B/R -> alpha: " << alfa << ", beta: " << beta << ", radius: " << radius << std::endl;
}


int main(int argc, char **argv) {
// init GLUT and the window

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH|GLUT_DOUBLE|GLUT_RGBA);
	
	
	
	
	
	glutInitWindowPosition(100,100);
	glutInitWindowSize(win_width,win_height);
	glutCreateWindow("Projeto CG-25");
		
// Required callback registry 
	glutDisplayFunc(renderScene);
	glutReshapeFunc(changeSize);
	glutIdleFunc(renderScene);
	
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
	std::cout << "Loading config file..." << std::endl; 
	if(!load_config_file(std::string("C:/Users/rafar/Desktop/Trabalho-CG-25/config_example.xml"))) return 1;
	std::cout << "Done!\n" << std::endl;

	glutReshapeWindow(win_width, win_height);
	cartesian2Spherical();
	
	printInfo();
	

	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

// enter GLUT's main cycle
	glutMainLoop();
	
	return 1;
}
