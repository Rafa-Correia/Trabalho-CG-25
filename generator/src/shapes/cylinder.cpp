#include "cylinder.hpp"

void cylinder_generator::generate(int argc, char **argv) const {
    if (argc != 7)
    {
        throw InvalidArgumentsException("Wrong number of arguments!");
    }
    std::string filepath(argv[6]);
    if (!validate_filepath(filepath))
    {
        throw InvalidArgumentsException("File is not valid!");
    }

    bool is_valid_radius = false;
    float radius;
    std::string radius_str(argv[2]);
    try
    {
        size_t pos;
        radius = std::stof(radius_str, &pos);
        is_valid_radius = pos == radius_str.length();
    }
    catch (const std::exception &)
    {
        is_valid_radius = false;
    }
    if (!is_valid_radius)
    {
        throw InvalidArgumentsException("Radius is not valid!");
    }
    else if (radius <= 0)
    {
        throw InvalidArgumentsException("Radius must be larger than 0!");
    }

    bool is_valid_height = false;
    float height;
    std::string height_str(argv[3]);
    try
    {
        size_t pos;
        height = std::stof(height_str, &pos);
        is_valid_height = pos == height_str.length();
    }
    catch (const std::exception &)
    {
        is_valid_height = false;
    }
    if (!is_valid_height)
    {
        throw InvalidArgumentsException("Height is not valid!");
    }
    else if (height <= 0)
    {
        throw InvalidArgumentsException("Height must be larger than 0!");
    }

    bool is_valid_slices = false;
    int slices;
    std::string slices_str(argv[4]);
    try
    {
        size_t pos;
        slices = std::stoi(slices_str, &pos);
        is_valid_slices = pos == slices_str.length();
    }
    catch (const std::exception &)
    {
        is_valid_slices = false;
    }
    if (!is_valid_slices)
    {
        throw InvalidArgumentsException("Number of slices is not valid!");
    }
    else if (slices <= 2)
    {
        throw InvalidArgumentsException("Number of slices must be larger than 2!");
    }

    bool is_valid_stacks = false;
    int stacks;
    std::string stacks_str(argv[5]);
    try
    {
        size_t pos;
        stacks = std::stoi(stacks_str, &pos);
        is_valid_stacks = pos == stacks_str.length();
    }
    catch (const std::exception &)
    {
        is_valid_stacks = false;
    }
    if (!is_valid_stacks)
    {
        throw InvalidArgumentsException("Number of stacks is not valid!");
    }
    else if (stacks <= 0)
    {
        throw InvalidArgumentsException("Number of stacks must be larger than 0!");
    }

    std::cout << "Radius: " << radius << std::endl;
    std::cout << "Heigth: " << height << std::endl;
    std::cout << "Slices: " << slices << std::endl;
    std::cout << "Stacks: " << stacks << std::endl;
    std::cout << "Filepath: " << filepath << std::endl;

    std::ofstream file(filepath);

    if (file.is_open())
    {
        std::vector<float> vertices;
        std::vector<size_t> indices;

        float angle_delta = (float)(2 * M_PI / slices);
        int i = 0;
        float alpha = angle_delta * i;

        // vertex 0 (center)
        vertices.push_back(0.0f); // x
        vertices.push_back(- (height / 2.0f)); // y
        vertices.push_back(0.0f); // z

        // vertex 1 (first)
        vertices.push_back(std::sin(alpha) * radius); // x
        vertices.push_back(- (height / 2.0f));                     // y
        vertices.push_back(std::cos(alpha) * radius); // z

        for (i = 2; i < slices + 1; i++)
        {
            alpha = angle_delta * (i - 1);
            // std::cout << "i: " << i << " / alpha: " << alpha << std::endl;
            // vertex i
            vertices.push_back(std::sin(alpha) * radius); // x
            vertices.push_back(- (height / 2.0f));                     // y
            vertices.push_back(std::cos(alpha) * radius); // z

            indices.push_back(0);
            indices.push_back(i);
            indices.push_back(i - 1);
        }

        // last triangle, not built by loop, all vertices are there, just need to add indices
        i--; // dont question it
        indices.push_back((size_t) 0);
        indices.push_back(1);
        indices.push_back(i);

        // BY THIS POINT, ONLY THE BASE IS GENERATED

        // sides!

        int j;
        int k;

        float height_delta;
        float cur_height;
        //radius_delta = radius / stacks;
        height_delta = height / stacks;

        for (j = 1; j <= stacks; j++)
        {
            k = 0;
            cur_height = height_delta * j -( height / 2.0f);
            //cur_radius = radius_delta * (stacks - j);

            alpha = angle_delta * k;

            vertices.push_back(std::sin(alpha) * radius); // x
            vertices.push_back(cur_height);                   // y
            vertices.push_back(std::cos(alpha) * radius); // z

            // this pattern holds for all walls
            indices.push_back(slices * j + k + 1);
            indices.push_back(slices * j + k);
            indices.push_back(slices * (j - 1) + k + 1);

            for (k = 1; k < slices; k++)
            {
                alpha = angle_delta * k;

                vertices.push_back(std::sin(alpha) * radius); // x
                vertices.push_back(cur_height);                   // y
                vertices.push_back(std::cos(alpha) * radius); // z

                indices.push_back(slices * j + k + 1);
                indices.push_back(slices * j + k);
                indices.push_back(slices * (j - 1) + k);

                indices.push_back(slices * j + k + 1);
                indices.push_back(slices * (j - 1) + k);
                indices.push_back(slices * (j - 1) + k + 1);
            }
            k--;
            indices.push_back(slices * j + 1);
            indices.push_back(slices * j + k + 1);
            indices.push_back(slices * (j - 1) + k + 1);
        }

        // top stack
        // by this point only the peak vertex is missing

        vertices.push_back(0.0f);
        vertices.push_back(height / 2.0f);
        vertices.push_back(0.0f);

        // create last triangles
        for (k = 0; k < slices - 1; k++)
        {
            indices.push_back(slices * (stacks + 1) + 1);
            indices.push_back(slices * (stacks) + k + 1);
            indices.push_back(slices * (stacks) + k + 2);
        }

        indices.push_back(slices * (stacks + 1) + 1);
        indices.push_back(slices * (stacks) + k + 1);
        indices.push_back(slices * (stacks) + 1);

        // write to file

        // settings

        //always generating with indices, with no normals and no tex coords
        file << "100\n";
        
        float sphere_radius = sqrtf(radius * radius + (height / 2.0f) * (height / 2.0f));

        file << "0;0;0;" << sphere_radius << "\n";

        // vertices
        float *vertices_array = vertices.data();
        file << vertices_array[0];
        for (i = 1; (long long unsigned int)i < vertices.size(); i++)
        {
            file << ";" << vertices_array[i];
        }
        file << "\n";
        
        // indices
        size_t *indices_array = indices.data();
        file << indices_array[0];
        for (i = 1; (long long unsigned int)i < indices.size(); i++)
        {
            file << ";" << indices_array[i];
        }
        
        file << std::flush;

        file.close();
    }
    else
    {
        throw InvalidArgumentsException("Error opening the file!");
    }
}