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

    // all arguments are initialized at this point!

    // std::cout << "Center radius: " << center_radius << std::endl;
    // std::cout << "Inner radius: " << inner_radius << std::endl;
    // std::cout << "Number of slices: " << n_slices << std::endl;
    // std::cout << "Number of sections: " << n_sections << std::endl;

    std::ofstream file(filepath);

    if (file.is_open())
    {
        std::vector<float> vertices;
        std::vector<size_t> indices;

        float alpha, beta;
        float alpha_delta = 2 * (float)M_PI / (float)n_slices;
        float beta_delta = 2 * (float)M_PI / (float)n_sections;

        // loop for all sections
        for (int i = 0; i < n_sections; i++)
        {
            beta = i * beta_delta;
            // loop to build each sections "circle"
            for (int j = 0; j < n_slices; j++)
            {
                alpha = j * alpha_delta;

                vertices.push_back(sinf(beta) * cosf(alpha) * inner_radius + sinf(beta) * (inner_radius + center_radius)); // x
                vertices.push_back(sinf(alpha) * inner_radius);                                                            // y
                vertices.push_back(cosf(beta) * cosf(alpha) * inner_radius + cosf(beta) * (inner_radius + center_radius)); // z

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
        std::cout << std::flush;

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
        file << "100\n";

        file << "0;0;0;" << center_radius + 2 * inner_radius << "\n";

        std::cout << "Metadata complete..." << std::endl;

        // vertices
        float *vertices_array = vertices.data();
        if (vertices.size() != 0)
            file << vertices_array[0];
        for (int i = 1; (long long unsigned int)i < vertices.size(); i++)
        {
            file << ";" << vertices_array[i];
        }
        file << "\n";

        // indices
        size_t *indices_array = indices.data();
        if (indices.size() != 0)
            file << indices_array[0];
        for (int i = 1; (long long unsigned int)i < indices.size(); i++)
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