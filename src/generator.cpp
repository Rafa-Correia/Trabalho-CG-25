#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>

#define UNIT_SIZE 1.0f

using namespace std;

bool validate_filepath(const std::string& filepath) {
    const std::string extention = ".3d";
    if(filepath.length() <= extention.length()) {
        return false;
    }
    return filepath.compare(filepath.length() - extention.length(), extention.length(), extention) == 0;
}

bool generate_plane(int argc, char **argv) {
    std::ofstream plane_file("plane.3d");

    if (plane_file.is_open()) {
        if (argc != 5) {
            std::cout << "Wrong number of arguments!" << std::endl;
            return false;
        }
        std::string filepath(argv[4]);
        if (!validate_filepath(filepath)) {
            std::cout << "Not valid" << std::endl;
            return false;
        }
        
        std::cout << "Length: " << length << std::endl;
        std::cout << "Divisons: " << divisions << std::endl;
        std::cout << "Filepath: " << filepath << std::endl;

        std::ofstream file(filepath);
        plane_file.close();
    }
    else {
        std::cerr << "Error opening file" << std::endl;
    }

    for (int i = -1; i < 2; i++) {
        for (int j = -1; j < 2; j++) {
            // Primeiro triângulo
            glVertex3f(-0.5f + i, 0.0f, -0.5f + j);
            glVertex3f(0.5f + i, 0.0f, 0.5f + j);
            glVertex3f(0.5f + i, 0.0f, -0.5f + j);

            // Segundo triângulo
            glVertex3f(-0.5f + i, 0.0f, -0.5f + j);
            glVertex3f(-0.5f + i, 0.0f, 0.5f + j);
            glVertex3f(0.5f + i, 0.0f, 0.5f + j);
        }
    }
}

int main(int argc, char **argv) {
    if (argc < 5 || argc > 7) {
        std::cout << "Wrong number of arguments." << std::endl;
        return 1;
    }

    std::string model_type(argv[1]);
    if(model_type.compare("sphere") == 0) {

    } 
    else if (model_type.compare("box") == 0) {

    } 
    else if(model_type.compare("cone") == 0) {

    }
    else if(model_type.compare("plane") == 0) {
        generate_plane(argc, **argv);
    }
    else {
        std::cout << "Invalid shape!" << std::endl;
        return 1;
    }

    return 0;
}