#include "generator/cone.hpp"

void cone_generator::generate(int argc, char **argv)
{
    if (argc != 7)
    {
        throw InvalidArgumentsException("Wrong number of arguments!");
    }
    std::string filepath(argv[6]);
    if (!cone_generator::validate_filepath(filepath))
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
        throw InvalidArgumentsException("Number of slices is not valid!");

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
        throw InvalidArgumentsException("Error opening output file!");
    }

    std::vector<vector3> vertices;
    std::vector<size_t> indices;

    std::vector<vector3> normals;

    float angle_delta = (float)(2 * M_PI / slices);
    float alpha = 0;

    // generate vertices, normals and indices

    // base
    vector3 center(0.0f, 0.0f, 0.0f);
    vector3 center_normal(0.0f, -1.0f, 0.0f);

    vertices.push_back(center);
    normals.push_back(center_normal);

    for (size_t i = 0; i < slices; i++)
    {
        alpha = angle_delta * i;
        vector3 v(sinf(alpha), 0.0f, cosf(alpha));
        v *= radius; // scale it up!

        vector3 n(0.0f, -1.0f, 0.0f); // normal, on base it just points down!

        size_t center_i = 0;
        size_t current_i = i + 1;
        size_t next_i = (i + 1) % slices + 1;

        vertices.push_back(v);
        normals.push_back(n);

        indices.push_back(current_i);
        indices.push_back(center_i);
        indices.push_back(next_i);
    }

    // sides
    size_t index_base = vertices.size();

    float height_delta = height / stacks;
    float radius_delta = radius / stacks;

    float hypotenuse = sqrtf(radius * radius + height * height);

    for (size_t i = 0; i < stacks; i++)
    {
        for (size_t j = 0; j < slices; j++)
        {
            alpha = angle_delta * j;
            float current_height = height_delta * i;
            float current_radius = radius_delta * (stacks - i);

            vector3 v(sinf(alpha) * current_radius, current_height, cosf(alpha) * current_radius);
            vector3 n = v;
            n.x /= (height / hypotenuse);
            n.y = radius / hypotenuse; // << EQUALS!!
            n.z /= (height / hypotenuse);
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

    // peak!

    vector3 peak(0.0f, height, 0.0f);
    vector3 peak_normal(0.0f, 1.0f, 0.0f);

    vertices.push_back(peak);
    normals.push_back(peak_normal);

    size_t peak_i = vertices.size() - 1;

    for (size_t i = 0; i < slices; i++)
    {
        size_t top_ring = index_base + (stacks - 1) * slices + i;
        size_t next_top = index_base + (stacks - 1) * slices + (i + 1) % slices;

        indices.push_back(peak_i);
        indices.push_back(top_ring);
        indices.push_back(next_top);
    }

    // write to file

    file << "110\n"; // << settings / metadata!

    float bound_radius;
    radius > height ? bound_radius = radius : bound_radius = height;

    file << "0;0;0;" << bound_radius << "\n";

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

    for (size_t j = 0; j < normals.size(); j++)
    {
        if (j != 0)
            file << ";";
        file << normals.at(j);
    }

    file << std::flush;

    file.close();
}