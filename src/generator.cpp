#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>

#define _USE_MATH_DEFINES
#include <math.h>

#define UNIT_SIZE 1.0f

using namespace std;

bool validate_filepath(const std::string& filepath) {
    const std::string extention = ".3d";
    if(filepath.length() <= extention.length()) {
        return false;
    }
    return filepath.compare(filepath.length() - extention.length(), extention.length(), extention) == 0;
}


/**
 * <---------------------------------------->
 * CONE VERTICES/INDICES GENERATING FUNCTION
 * <---------------------------------------->
 */
bool generate_cone(int argc, char** argv) {
    if(argc != 7) {
        std::cout << "Wrong number of arguments!" << std::endl;
        return false;
    }
    std::string filepath(argv[6]);
    if(!validate_filepath(filepath)) {
        std::cout << "File is not valid!" << std::endl;
        return false;
    }


    bool is_valid_radius = false;
    float radius;
    std::string radius_str(argv[2]);
    try {
        size_t pos;
        radius = std::stof(radius_str, &pos);
        is_valid_radius = pos == radius_str.length();
    } catch (const std::exception&) {
        is_valid_radius = false;
    }
    if(!is_valid_radius) {
        std::cout << "Radius is not valid!" << std::endl;
        return false;
    }
    else if(radius <= 0) {
        std::cout << "Radius must be larger than 0!" << std::endl;
        return false;
    }

    bool is_valid_height = false;
    float height;
    std::string height_str(argv[3]);
    try {
        size_t pos;
        height = std::stof(height_str, &pos);
        is_valid_height = pos == height_str.length();
    } catch (const std::exception&) {
        is_valid_height = false;
    }
    if(!is_valid_height) {
        std::cout << "Height is not valid!" << std::endl;
        return false;
    }
    else if(height <= 0) {
        std::cout << "Height must be larger than 0!" << std::endl;
        return false;
    }

    bool is_valid_slices = false;
    int slices;
    std::string slices_str(argv[4]);
    try {
        size_t pos;
        slices = std::stoi(slices_str, &pos);
        is_valid_slices = pos == slices_str.length();
    } catch (const std::exception&) {
        is_valid_slices = false;
    }
    if(!is_valid_slices) {
        std::cout << "Number of slices is not valid!" << std::endl;
        return false;
    }
    else if(slices <= 2) {
        std::cout << "Number of slices must be larger than 2!" << std::endl;
        return false;
    }
    
    bool is_valid_stacks = false;
    int stacks;
    std::string stacks_str(argv[5]);
    try {
        size_t pos;
        stacks = std::stoi(stacks_str, &pos);
        is_valid_stacks = pos == stacks_str.length();
    } catch (const std::exception&) {
        is_valid_stacks = false;
    }
    if(!is_valid_stacks) {
        std::cout << "Number of stacks is not valid!" << std::endl;
        return false;
    }
    else if(stacks <= 0) {
        std::cout << "Number of stacks must be larger than 0!" << std::endl;
        return false;
    }

    std::cout << "Radius: " << radius << std::endl;
    std::cout << "Heigth: " << height << std::endl;
    std::cout << "Slices: " << slices << std::endl;
    std::cout << "Stacks: " << stacks << std::endl;
    std::cout << "Filepath: " << filepath << std::endl;

    std::ofstream file(filepath);

    if (file.is_open()) {
        std::vector<float> vertices;
        std::vector<size_t> indices;

        float angle_delta = 2 * M_PI / slices;
        int i = 0;
        float alpha = angle_delta * i;

        //vertex 0 (center)
        vertices.push_back(0.0f);   //x
        vertices.push_back(0.0f);   //y
        vertices.push_back(0.0f);   //z

        //vertex 1 (first)
        vertices.push_back(std::sin(alpha) * radius);   //x
        vertices.push_back(0.0f);                       //y
        vertices.push_back(std::cos(alpha) * radius);   //z

        for(i = 2; i < slices + 1; i++) {
            alpha = angle_delta * (i - 1);
            //std::cout << "i: " << i << " / alpha: " << alpha << std::endl;
            //vertex i
            vertices.push_back(std::sin(alpha) * radius);   //x
            vertices.push_back(0.0f);                       //y
            vertices.push_back(std::cos(alpha) * radius);   //z

            indices.push_back(0);
            indices.push_back(i);
            indices.push_back(i-1);
        }

        //last triangle, not built by loop, all vertices are there, just need to add indices
        i--; //dont question it
        indices.push_back(0);
        indices.push_back(1);
        indices.push_back(i);

        //BY THIS POINT, ONLY THE BASE IS GENERATED

        //sides!
        
        int j;
        int k;

        float radius_delta, height_delta;
        float cur_radius, cur_height;
        radius_delta = radius / stacks;
        height_delta = height / stacks;

        for(j = 1; j < stacks; j++) {
            k = 0;
            cur_height = height_delta * j;
            cur_radius = radius_delta * (stacks - j);

            alpha = angle_delta * k;

            vertices.push_back(std::sin(alpha) * cur_radius);   //x
            vertices.push_back(cur_height);                     //y
            vertices.push_back(std::cos(alpha) * cur_radius);   //z

            //this pattern holds for all walls
            indices.push_back(slices*j + k + 1);     
            indices.push_back(slices*j + k);
            indices.push_back(slices*(j-1) + k + 1);

            for(k = 1; k < slices; k++) {
                alpha = angle_delta * k;

                vertices.push_back(std::sin(alpha) * cur_radius);   //x
                vertices.push_back(cur_height);                     //y
                vertices.push_back(std::cos(alpha) * cur_radius);   //z

                indices.push_back(slices*j + k + 1);     
                indices.push_back(slices*j + k);
                indices.push_back(slices*(j-1) + k);

                indices.push_back(slices*j + k + 1);
                indices.push_back(slices*(j-1) + k);
                indices.push_back(slices*(j-1) + k + 1);
            }
            k--;
            indices.push_back(slices*j + 1);
            indices.push_back(slices*j + k + 1);
            indices.push_back(slices*(j-1) + k + 1);
        }

        //top stack
        //by this point only the peak vertex is missing

        vertices.push_back(0.0f);
        vertices.push_back(height);
        vertices.push_back(0.0f);

        //create last triangles
        for(k = 0; k < slices - 1; k++) {
            indices.push_back(slices * stacks + 1);
            indices.push_back(slices * (stacks - 1) + k + 1);
            indices.push_back(slices * (stacks - 1) + k + 2);
        }

        indices.push_back(slices * stacks + 1);
        indices.push_back(slices * (stacks - 1) + k + 1);
        indices.push_back(slices * (stacks - 1) + 1);

        //write to file

        //settings
        file << "100\n";
        //indices first
        size_t *indices_array = indices.data();
        file << indices_array[0];
        for(i = 1; (long long unsigned int)i < indices.size(); i++) {
            file << ";" << indices_array[i];
        }
        file << "\n";

        //vertices
        float *vertices_array = vertices.data();
        file << vertices_array[0];
        for(i = 1; (long long unsigned int)i < vertices.size(); i++) {
            file << ";" << vertices_array[i];
        }

        file.close();
    }
    else {
        std::cout << "Error opening the file!" << std::endl;
        return false;
    }

    return true;
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
        
        //std::cout << "Length: " << length << std::endl;
        //std::cout << "Divisons: " << divisions << std::endl;
        std::cout << "Filepath: " << filepath << std::endl;

        std::ofstream file(filepath);
        plane_file.close();
        return true;
    }
    else {
        std::cerr << "Error opening file" << std::endl;
    }

    /*
    for (int i = -1; i < 2; i++) {
        for (int j = -1; j < 2; j++) {
            // Primeiro tri�ngulo
            glVertex3f(-0.5f + i, 0.0f, -0.5f + j);
            glVertex3f(0.5f + i, 0.0f, 0.5f + j);
            glVertex3f(0.5f + i, 0.0f, -0.5f + j);

            // Segundo tri�ngulo
            glVertex3f(-0.5f + i, 0.0f, -0.5f + j);
            glVertex3f(-0.5f + i, 0.0f, 0.5f + j);
            glVertex3f(0.5f + i, 0.0f, 0.5f + j);
        }
    }
    */
   return true;
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
        if(!generate_cone(argc, argv)) {
            return 1;
        }
    }
    else if(model_type.compare("plane") == 0) {
        if(!generate_plane(argc, argv)) {
            return 1;
        }
    }
    else {
        std::cout << "Invalid shape!" << std::endl;
        return 1;
    }

    return 0;
}