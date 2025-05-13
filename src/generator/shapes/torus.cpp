#include "generator/torus.hpp"

void torus_generator::generate(int argc, char **argv)
{
    // args -> -name- torus center_radius inner_radius n_slices n_sections file
    if (argc != 7)
    {
        throw InvalidArgumentsException("Wrong number of arguments!");
    }

    std::string filepath(argv[6]);
    if (!validate_filepath(filepath))
    {
        throw InvalidArgumentsException("File is not valid!");
    }

    float center_radius;
    try
    {
        center_radius = std::stof(argv[2]);
    }
    catch (const std::exception &)
    {
        throw InvalidArgumentsException("Center radius is not valid!");
    }
    if (center_radius <= 0)
        throw InvalidArgumentsException("Center radius must be larger than 0!");

    float inner_radius;
    try
    {
        inner_radius = std::stof(argv[3]);
    }
    catch (const std::exception &)
    {
        throw InvalidArgumentsException("Inner radius is not valid!");
    }
    if (inner_radius <= 0)
        throw InvalidArgumentsException("Inner radius must be larger than 0!");

    int n_slices;
    try
    {
        n_slices = std::stoi(argv[4]);
    }
    catch (const std::exception &)
    {
        throw InvalidArgumentsException("Number of slices is not valid!");
    }
    if (n_slices < 3)
        throw InvalidArgumentsException("Number of slices must be larger than or equal to 3!");

    int n_sections;
    try
    {
        n_sections = std::stoi(argv[5]);
    }
    catch (const std::exception &)
    {
        throw InvalidArgumentsException("Number of sections is not valid!");
    }
    if (n_sections < 3)
        throw InvalidArgumentsException("Number of sections must be larger than or equal to 3!");

    std::ofstream file(filepath);

    if (!file.is_open())
    {
        throw InvalidArgumentsException("Error opening the file!");
    }

    std::vector<vector3> vertices;
    std::vector<size_t> indices;

    std::vector<vector3> normals;
    std::vector<vector2> tex_coords;

    float alpha, beta;
    float alpha_delta = 2 * (float)M_PI / (float)n_slices;
    float beta_delta = 2 * (float)M_PI / (float)n_sections;

    // loop for all sections
    for (int i = 0; i < n_sections; i++)
    {
        beta = i * beta_delta;
        float u_tex = beta / (2.0f * M_PI);
        // loop to build each section's "circle"
        for (int j = 0; j < n_slices; j++)
        {
            alpha = j * alpha_delta;
            float v_tex = alpha / (2.0f * M_PI);

            vector3 v(
                sinf(beta) * cosf(alpha) * inner_radius + sinf(beta) * (inner_radius + center_radius),
                sinf(alpha) * inner_radius,
                cosf(beta) * cosf(alpha) * inner_radius + cosf(beta) * (inner_radius + center_radius));

            vertices.push_back(v);

            vector3 center(
                sinf(beta) * (inner_radius + center_radius),
                0.0f,
                cosf(beta) * (inner_radius + center_radius));

            vector3 n = v - center;
            n.normalize();
            normals.push_back(n);

            vector2 t(u_tex, v_tex);
            tex_coords.push_back(t);

            if (i != 0)
            {
                indices.push_back(i * n_slices + j);
                indices.push_back((i - 1) * n_slices + ((j + 1) % n_slices));
                indices.push_back((i - 1) * n_slices + j);

                if (j == 0)
                    continue;
                indices.push_back(i * n_slices + j);
                indices.push_back((i - 1) * n_slices + j);
                indices.push_back(i * n_slices + j - 1);
            }
        }

        if (i == 0)
            continue;

        indices.push_back(i * n_slices);
        indices.push_back((i - 1) * n_slices);
        indices.push_back((i + 1) * n_slices - 1);
    }

    // build last section here!

    for (int j = 0; j < n_slices; j++)
    {
        indices.push_back(j);
        indices.push_back((n_sections - 1) * n_slices + ((j + 1) % n_slices));
        indices.push_back((n_sections - 1) * n_slices + j);

        if (j == 0)
            continue;
        indices.push_back(j);
        indices.push_back((n_sections - 1) * n_slices + j);
        indices.push_back(j - 1);
    }

    indices.push_back(0);
    indices.push_back((n_sections - 1) * n_slices);
    indices.push_back(n_sections - 1);

    // write to file
    file << "111\n";

    file << "0;0;0;" << center_radius + 2 * inner_radius << "\n";

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

    for (size_t i = 0; i < normals.size(); i++)
    {
        if (i != 0)
            file << ";";
        file << normals.at(i);
    }
    file << "\n";

    for (size_t i = 0; i < tex_coords.size(); i++)
    {
        if (i != 0)
            file << ";";
        file << tex_coords.at(i);
    }

    file << std::flush;

    file.close();
}