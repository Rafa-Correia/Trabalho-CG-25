#include "generator/box.hpp"

void box_generator::generate(int argc, char **argv) const
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

    float size = std::stof(argv[2]);
    int divisions = std::stoi(argv[3]);

    if (size <= 0 || divisions < 1)
    {
        throw InvalidArgumentsException("Invalid size or divisions!");
    }

    std::ofstream file(filepath);
    if (!file.is_open())
    {
        throw InvalidArgumentsException("Error opening the file!");
    }

    std::vector<float> vertices;
    std::vector<size_t> indices;

    float halfSize = size / 2.0f;
    float step = size / divisions;

    // Função para adicionar uma face
    auto addFace = [&](float nx, float ny, float nz, // Normal direction
                       float ox, float oy, float oz, // Origin
                       float ux, float uy, float uz, // Horizontal vector (U)
                       float vx, float vy, float vz, // Vertical vector (V)
                       bool flip = false) {          // Invert triangle order if needed
        int baseIndex = vertices.size() / 3;

        for (int i = 0; i <= divisions; i++)
        {
            for (int j = 0; j <= divisions; j++)
            {
                float x = ox + j * ux + i * vx;
                float y = oy + j * uy + i * vy;
                float z = oz + j * uz + i * vz;

                vertices.push_back(x);
                vertices.push_back(y);
                vertices.push_back(z);

                if (i < divisions && j < divisions)
                {
                    int current = baseIndex + i * (divisions + 1) + j;
                    int nextRow = baseIndex + (i + 1) * (divisions + 1) + j;

                    if (flip)
                    {
                        // Inverted winding order
                        indices.push_back(current);
                        indices.push_back(nextRow + 1);
                        indices.push_back(nextRow);

                        indices.push_back(current);
                        indices.push_back(current + 1);
                        indices.push_back(nextRow + 1);
                    }
                    else
                    {
                        // Normal winding order
                        indices.push_back(current);
                        indices.push_back(nextRow);
                        indices.push_back(nextRow + 1);

                        indices.push_back(current);
                        indices.push_back(nextRow + 1);
                        indices.push_back(current + 1);
                    }
                }
            }
        }
    };

    // Faces (com vetores e origens ajustadas)

    addFace(0, 1, 0, -halfSize, -halfSize, -halfSize, step, 0, 0, 0, 0, step, true);

    addFace(0, -1, 0, -halfSize, halfSize, -halfSize, step, 0, 0, 0, 0, step);

    addFace(0, 0, -1, -halfSize, -halfSize, halfSize, step, 0, 0, 0, step, 0, true);

    addFace(0, 0, 1, halfSize, -halfSize, -halfSize, -step, 0, 0, 0, step, 0, true);

    addFace(1, 0, 0, -halfSize, -halfSize, -halfSize, 0, 0, step, 0, step, 0, true);

    addFace(-1, 0, 0, halfSize, -halfSize, halfSize, 0, 0, -step, 0, step, 0, true);

    // Escreve no ficheiro
    file << "100\n"; // Placeholder de settings, pode ajustar se necessário

    file << "0;0;0;" << (size * sqrtf(3)) / 2.0f << "\n";

    // Escreve vértices
    for (size_t i = 0; i < vertices.size(); i++)
    {
        if (i > 0)
            file << ";";
        file << vertices[i];
    }
    file << "\n";
    // Escreve índices
    for (size_t i = 0; i < indices.size(); i++)
    {
        if (i > 0)
            file << ";";
        file << indices[i];
    }

    file << std::flush;

    file.close();
}