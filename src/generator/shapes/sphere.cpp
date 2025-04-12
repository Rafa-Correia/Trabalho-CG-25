#include "generator/sphere.hpp"

void sphere_generator::generate(int argc, char **argv)
{
    if (argc != 6)
    {
        throw InvalidArgumentsException("Wrong number of arguments!");
    }
    std::string filepath(argv[5]);
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

    bool is_valid_slices = false;
    int slices;
    std::string slices_str(argv[3]);
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
    std::string stacks_str(argv[4]);
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

    // std::cout << "Radius: " << radius << std::endl;
    // std::cout << "Slices: " << slices << std::endl;
    // std::cout << "Stacks: " << stacks << std::endl;
    // std::cout << "Filepath: " << filepath << std::endl;

    std::ofstream file(filepath);

    if (file.is_open())
    {
        std::vector<float> vertices;
        std::vector<size_t> indices;

        float angle_delta = (float)(2 * M_PI / slices);
        float beta_delta = (float)(M_PI / (stacks + 1));
        int i = 0;
        int j = 0;
        int k = 0;
        float alpha = angle_delta * i;
        float beta = beta_delta * (j + 1);

        // vertex 0 (center)
        vertices.push_back(0.0f);    // x
        vertices.push_back(-radius); // y
        vertices.push_back(0.0f);    // z

        // vertex 1 (first)
        vertices.push_back(sinf(alpha) * cosf(beta - RIGHT_ANGLE) * radius); // x
        vertices.push_back(sinf(beta - RIGHT_ANGLE) * radius);               // y
        vertices.push_back(cosf(alpha) * cosf(beta - RIGHT_ANGLE) * radius); // z

        for (i = 2; i < slices + 1; i++)
        {
            alpha = angle_delta * (i - 1);
            // std::cout << "i: " << i << " / alpha: " << alpha << std::endl;
            // vertex i
            vertices.push_back(std::sin(alpha) * std::cos(beta - RIGHT_ANGLE) * radius); // x
            vertices.push_back(std::sin(beta - RIGHT_ANGLE) * radius);                   // y
            vertices.push_back(std::cos(alpha) * std::cos(beta - RIGHT_ANGLE) * radius); // z

            indices.push_back(0);
            indices.push_back(i);
            indices.push_back(i - 1);
        }

        // last triangle, not built by loop, all vertices are there, just need to add indices
        i--; // dont question it
        indices.push_back(0);
        indices.push_back(1);
        indices.push_back(i);

        // BY THIS POINT, ONLY THE BASE IS GENERATED

        // sides!

        float radius_delta, height_delta;
        float cur_radius, cur_height;
        radius_delta = radius / stacks;
        height_delta = (float)((float)1 / stacks);

        for (j = 1; j < stacks + 1; j++)
        {
            k = 0;
            alpha = angle_delta * k;
            beta = beta_delta * (j + 1);

            cur_height = radius * std::sin(beta - RIGHT_ANGLE);
            cur_radius = radius * std::cos(beta - RIGHT_ANGLE);

            vertices.push_back(std::sin(alpha) * cur_radius); // x
            vertices.push_back(cur_height);                   // y
            vertices.push_back(std::cos(alpha) * cur_radius); // z

            // this pattern holds for all walls
            indices.push_back(slices * j + k + 1);
            indices.push_back(slices * j + k);
            indices.push_back(slices * (j - 1) + k + 1);

            for (k = 1; k < slices; k++)
            {
                alpha = angle_delta * k;

                vertices.push_back(std::sin(alpha) * cur_radius); // x
                vertices.push_back(cur_height);                   // y
                vertices.push_back(std::cos(alpha) * cur_radius); // z

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
        vertices.push_back(radius);
        vertices.push_back(0.0f);

        // create last triangles
        for (k = 0; k < slices - 1; k++)
        {
            indices.push_back(slices * stacks + 1);
            indices.push_back(slices * (stacks - 1) + k + 1);
            indices.push_back(slices * (stacks - 1) + k + 2);
        }

        indices.push_back(slices * stacks + 1);
        indices.push_back(slices * (stacks - 1) + k + 1);
        indices.push_back(slices * (stacks - 1) + 1);

        // write to file

        // settings
        file << "100\n";

        file << "0;0;0;" << radius << "\n";

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