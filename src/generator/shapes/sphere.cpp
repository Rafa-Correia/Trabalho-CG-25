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

    float radius;
    try
    {
        size_t pos;
        radius = std::stof(argv[2]);
    }
    catch (const std::exception &)
    {
        throw InvalidArgumentsException("Radius is not valid!");
    }
    if (radius <= 0)
        throw InvalidArgumentsException("Radius must be larger than 0!");

    int slices;
    try
    {
        slices = std::stoi(argv[3]);
    }
    catch (const std::exception &)
    {
        throw InvalidArgumentsException("Number of slices is not valid!");
    }
    if (slices <= 2)
        throw InvalidArgumentsException("Number of slices must be larger than 2!");

    int stacks;
    try
    {
        stacks = std::stoi(argv[4]);
    }
    catch (const std::exception &)
    {
        throw InvalidArgumentsException("Number of stacks is not valid!");
    }
    if (stacks <= 0)
        throw InvalidArgumentsException("Number of stacks must be larger than 0!");

    // std::cout << "Radius: " << radius << std::endl;
    // std::cout << "Slices: " << slices << std::endl;
    // std::cout << "Stacks: " << stacks << std::endl;
    // std::cout << "Filepath: " << filepath << std::endl;

    std::ofstream file(filepath);

    if (!file.is_open())
    {
        throw InvalidArgumentsException("Error opening the file!");
    }

    std::vector<vector3> vertices;
    std::vector<size_t> indices;

    std::vector<vector3> normals;

    float angle_delta = (float)(2 * M_PI / slices);
    float beta_delta = (float)(M_PI / (stacks + 1));
    int i = 0;
    int j = 0;
    int k = 0;
    float alpha = angle_delta * i;
    float beta = beta_delta * (j + 1);

    // vertex 0 (center)
    vector3 v0(0, -radius, 0);
    vertices.push_back(v0);

    vector3 n0 = v0;
    n0.normalize();
    normals.push_back(n0);

    // vertex 1 (first)
    vector3 v1(sinf(alpha) * cosf(beta - RIGHT_ANGLE) * radius, sinf(beta - RIGHT_ANGLE) * radius, cosf(alpha) * cosf(beta - RIGHT_ANGLE) * radius);
    vertices.push_back(v1);

    vector3 n1 = v1;
    n1.normalize();
    normals.push_back(v1);

    for (i = 2; i < slices + 1; i++)
    {
        alpha = angle_delta * (i - 1);
        // std::cout << "i: " << i << " / alpha: " << alpha << std::endl;
        // vertex i
        vector3 v(sinf(alpha) * cosf(beta - RIGHT_ANGLE) * radius, sinf(beta - RIGHT_ANGLE) * radius, cosf(alpha) * cosf(beta - RIGHT_ANGLE) * radius);
        vertices.push_back(v);

        vector3 n = v;
        n.normalize();
        normals.push_back(n);

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

        cur_height = radius * sinf(beta - RIGHT_ANGLE);
        cur_radius = radius * cosf(beta - RIGHT_ANGLE);

        vector3 v_first(sinf(alpha) * cur_radius, cur_height, cosf(alpha) * cur_radius);
        vertices.push_back(v_first);

        vector3 n_first = v_first;
        n_first.normalize();
        normals.push_back(n_first);

        // this pattern holds for all walls
        indices.push_back(slices * j + k + 1);
        indices.push_back(slices * j + k);
        indices.push_back(slices * (j - 1) + k + 1);

        for (k = 1; k < slices; k++)
        {
            alpha = angle_delta * k;

            vector3 v(sinf(alpha) * cur_radius, cur_height, cosf(alpha) * cur_radius);
            vertices.push_back(v);

            vector3 n = v;
            n.normalize();
            normals.push_back(n);

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

    vector3 v_last(0, radius, 0);
    vertices.push_back(v_last);

    vector3 n_last = v_last;
    n_last.normalize();
    normals.push_back(n_last);

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
    file << "110\n";

    file << "0;0;0;" << radius << "\n";

    // vertices
    for (size_t j = 0; j < vertices.size(); j++)
    {
        if (j != 0)
            file << ";";
        file << vertices.at(j);
    }
    file << "\n";

    // indices
    for (size_t j = 0; j < indices.size(); j++)
    {
        if (j != 0)
            file << ";";
        file << indices.at(j);
    }
    file << "\n";

    // normals
    for (size_t j = 0; j < normals.size(); j++)
    {
        if (j != 0)
            file << ";";
        file << normals.at(j);
    }

    file << std::flush;

    file.close();
}