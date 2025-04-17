#include "generator/cylinder.hpp"

void cylinder_generator::generate(int argc, char **argv)
{
    if (argc != 7)
    {
        throw InvalidArgumentsException("Wrong number of arguments!");
    }
    std::string filepath(argv[6]);
    if (!validate_filepath(filepath))
    {
        throw InvalidArgumentsException("File is not valid!");
    }

    float radius;
    try
    {
        radius = std::stof(argv[2]);
    }
    catch (const std::exception &)
    {
        throw InvalidArgumentsException("Radius is not valid!");
    }
    if (radius <= 0)
        throw InvalidArgumentsException("Radius must be larger than 0!");

    float height;
    try
    {
        height = std::stof(argv[3]);
    }
    catch (const std::exception &)
    {
        throw InvalidArgumentsException("Height is not valid!");
    }
    if (height <= 0)
        throw InvalidArgumentsException("Height must be larger than 0!");

    int slices;
    try
    {
        slices = std::stoi(argv[4]);
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
        stacks = std::stoi(argv[5]);
    }
    catch (const std::exception &)
    {
        throw InvalidArgumentsException("Number of stacks is not valid!");
    }
    if (stacks <= 0)
        throw InvalidArgumentsException("Number of stacks must be larger than 0!");

    // std::cout << "Radius: " << radius << std::endl;
    // std::cout << "Heigth: " << height << std::endl;
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
    float alpha;

    // vertex 0 (center)
    vector3 v0(0.0f, -(height / 2.0f), 0.0f);
    vertices.push_back(v0);

    vector3 n0(0.0f, -1.0f, 0.0f);
    normals.push_back(n0);

    for (size_t i = 0; i < slices; i++)
    {
        alpha = angle_delta * i;

        vector3 v(sinf(alpha) * radius, -(height / 2.0f), cosf(alpha) * radius);
        vector3 n(0.0f, -1.0f, 0.0f);

        size_t center_i = 0;
        size_t current_i = i + 1;
        size_t next_i = (i + 1) % slices + 1;

        vertices.push_back(v);
        normals.push_back(n);

        indices.push_back(current_i);
        indices.push_back(center_i);
        indices.push_back(next_i);
    }

    // sides!
    float height_delta;
    float cur_height;
    height_delta = (height / stacks);
    size_t index_base = vertices.size();

    for (size_t i = 0; i <= stacks; i++)
    {
        cur_height = height_delta * i - (height / 2.0f);
        for (size_t j = 0; j < slices; j++)
        {
            alpha = angle_delta * j;

            vector3 v(sinf(alpha) * radius, cur_height, cosf(alpha) * radius);
            vector3 n = v;
            n.y = 0;
            n.normalize();

            vertices.push_back(v);
            normals.push_back(n);

            if (i == 0)
                continue;
            size_t this_current_stack = index_base + i * slices + j;
            size_t next_current_stack = index_base + i * slices + ((j + 1) % (slices));
            size_t this_prev_stack = index_base + (i - 1) * slices + j;
            size_t next_prev_stack = index_base + (i - 1) * slices + ((j + 1) % (slices));

            indices.push_back(this_current_stack);
            indices.push_back(this_prev_stack);
            indices.push_back(next_prev_stack);

            indices.push_back(this_current_stack);
            indices.push_back(next_prev_stack);
            indices.push_back(next_current_stack);
        }
    }

    index_base = vertices.size();

    // vertex 0 (center)
    vector3 v0_top(0.0f, (height / 2.0f), 0.0f);
    vertices.push_back(v0_top);

    vector3 n0_top(0.0f, 1.0f, 0.0f);
    normals.push_back(n0_top);

    for (size_t i = 0; i < slices; i++)
    {
        alpha = angle_delta * i;

        vector3 v(sinf(alpha) * radius, (height / 2.0f), cosf(alpha) * radius);
        vector3 n(0.0f, 1.0f, 0.0f);

        size_t center_i = index_base + 0;
        size_t current_i = index_base + i + 1;
        size_t next_i = index_base + (i + 1) % slices + 1;

        vertices.push_back(v);
        normals.push_back(n);

        indices.push_back(center_i);
        indices.push_back(current_i);
        indices.push_back(next_i);
    }

    // write to file

    // settings

    // always generating with indices, with no normals and no tex coords
    file << "110\n";

    float sphere_radius = sqrtf(radius * radius + (height / 2.0f) * (height / 2.0f));

    file << "0;0;0;" << sphere_radius << "\n";

    // vertices
    for (size_t i = 0; i < vertices.size(); i++)
    {
        if (i != 0)
            file << ";";
        file << vertices.at(i);
    }
    file << "\n";

    // indices
    for (size_t i = 0; i < indices.size(); i++)
    {
        if (i != 0)
            file << ";";
        file << indices.at(i);
    }
    file << "\n";

    // normals
    for (size_t i = 0; i < normals.size(); i++)
    {
        if (i != 0)
            file << ";";
        file << normals.at(i);
    }

    file << std::flush;

    file.close();
}