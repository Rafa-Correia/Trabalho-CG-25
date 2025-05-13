#include "generator/plane.hpp"

void plane_generator::generate(int argc, char **argv)
{
    if (argc != 5)
    {
        throw InvalidArgumentsException("Wrong number of arguments!");
    }

    std::string filepath(argv[4]);
    if (!validate_filepath(filepath))
    {
        throw InvalidArgumentsException("File is not valid!");
    }

    float length = std::stof(argv[2]);
    int divisions = std::stoi(argv[3]);

    if (length <= 0 || divisions < 1)
    {
        throw InvalidArgumentsException("Invalid length or divisions!");
    }

    std::ofstream file(filepath);
    if (!file.is_open())
    {
        throw InvalidArgumentsException("Error opening the file!");
    }

    std::vector<vector3> vertices;
    std::vector<size_t> indices;

    std::vector<vector3> normals;
    std::vector<vector2> tex_coords;

    float halfSize = length / 2.0f;
    float step = length / divisions;

    // Função para adicionar uma face
    auto addFace = [&](float nx, float ny, float nz, // Normal direction
                       float ox, float oy, float oz, // Origin
                       float ux, float uy, float uz, // Horizontal vector (U)
                       float vx, float vy, float vz  // Vertical vector (V)
                   ) {                               // Invert triangle order if needed
        int baseIndex = vertices.size() / 3;

        for (int i = 0; i <= divisions; i++)
        {
            for (int j = 0; j <= divisions; j++)
            {
                float x = ox + j * ux + i * vx;
                float y = 0.0f;
                float z = oz + j * uz + i * vz;

                vector3 v(x, y, z);
                vertices.push_back(v);

                vector3 n(nx, ny, nz);
                normals.push_back(n);

                vector2 t((float)j / divisions, (float)i / divisions);
                tex_coords.push_back(t);

                if (i < divisions && j < divisions)
                {
                    int current = baseIndex + i * (divisions + 1) + j;
                    int nextRow = baseIndex + (i + 1) * (divisions + 1) + j;

                    indices.push_back(current);
                    indices.push_back(nextRow);
                    indices.push_back(nextRow + 1);

                    indices.push_back(current);
                    indices.push_back(nextRow + 1);
                    indices.push_back(current + 1);
                }
            }
        };
    };

    addFace(0, 1, 0, -halfSize, -halfSize, -halfSize, step, 0, 0, 0, 0, step);

    file << "111\n"; // Placeholder de settings, pode ajustar se necessário

    file << "0;0;0;" << (length * sqrtf(2)) / 2.0f << "\n";

    for (size_t i = 0; i < vertices.size(); i++)
    {
        if (i != 0)
            file << ";";
        file << vertices.at(i);
    }
    file << "\n";

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