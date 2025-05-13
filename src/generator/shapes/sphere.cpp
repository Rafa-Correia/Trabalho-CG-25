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
    std::vector<vector2> tex_coords;

    float alpha_delta = (2.0f * M_PI) / slices;
    float beta_delta = M_PI / stacks;

    // Bottom pole
    vector3 v0(0.0f, -radius, 0.0f);
    vertices.push_back(v0);

    vector3 n0 = v0;
    n0.normalize();
    normals.push_back(n0);

    vector2 t0(0.5f, 1.0f);
    tex_coords.push_back(t0);

    for (size_t i = 1; i < stacks; i++)
    {
        float beta = beta_delta * i - RIGHT_ANGLE;

        for (size_t j = 0; j <= slices; j++)
        {
            float alpha = j * alpha_delta;

            float x = sinf(alpha) * cosf(beta);
            float y = sinf(beta);
            float z = cosf(alpha) * cosf(beta);

            vector3 v = vector3(x, y, z) * radius;
            vertices.push_back(v);

            vector3 n = v;
            n.normalize();
            normals.push_back(n);

            /* float u = (float)j / slices;
            float v_tex = 1.0f - ((sinf(beta) + 1.0f) / 2.0f);
            tex_coords.push_back(vector2(u, v_tex)); */

            float u_tex = alpha / (2 * M_PI);
            float v_tex = ((M_PI / 2.0f) - beta) / M_PI;
            tex_coords.push_back(vector2(u_tex, v_tex));
        }
    }

    vector3 v_last(0.0f, radius, 0.0f);
    vertices.push_back(v_last);

    vector3 n_last = v_last;
    n_last.normalize();
    normals.push_back(n_last);

    vector2 t_last(0.5f, 0.0f);
    tex_coords.push_back(t_last);

    size_t index_base = 1;

    // indexing first stack
    for (size_t j = 0; j < slices; j++)
    {
        size_t current = index_base + j;
        size_t next = index_base + j + 1;

        indices.push_back(0);
        indices.push_back(next);
        indices.push_back(current);
    }

    // indexing every stack between the first and last
    for (size_t i = 0; i < stacks - 2; i++)
    {
        for (size_t j = 0; j < slices; j++)
        {
            size_t row1 = index_base + i * (slices + 1);
            size_t row2 = index_base + (i + 1) * (slices + 1);

            size_t a = row1 + j;
            size_t b = row2 + j;
            size_t c = row2 + j + 1;
            size_t d = row1 + j + 1;

            indices.push_back(a);
            indices.push_back(c);
            indices.push_back(b);

            indices.push_back(a);
            indices.push_back(d);
            indices.push_back(c);
        }
    }

    // indexing last stack
    size_t top_index = vertices.size() - 1;
    size_t last_ring_start = index_base + (stacks - 2) * (slices + 1);
    for (size_t j = 0; j < slices; j++)
    {
        size_t current = last_ring_start + j;
        size_t next = last_ring_start + j + 1;

        indices.push_back(current);
        indices.push_back(next);
        indices.push_back(top_index);
    }

    // write to file

    // settings
    file << "111\n";

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

    file << "\n";

    // tex_coords
    for (size_t j = 0; j < tex_coords.size(); j++)
    {
        if (j != 0)
            file << ";";
        file << tex_coords.at(j);
    }

    file << std::flush;

    file.close();
}