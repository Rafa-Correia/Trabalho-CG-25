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

    /*     std::vector<vector3> vertices;
        std::vector<size_t> indices;

        std::vector<vector3> normals;
        std::vector<vector2> tex_coords;

        float angle_delta = (float)(2 * M_PI / slices);
        float alpha = 0;

        // generate vertices, normals and indices

        // base
        vector3 center(0.0f, 0.0f, 0.0f);
        vector3 center_normal(0.0f, -1.0f, 0.0f);
        vector2 center_tex_coord(0.5f, 1.0f);

        vertices.push_back(center);
        normals.push_back(center_normal);

        for (size_t i = 0; i < slices; i++)
        {
            alpha = angle_delta * i;
            vector3 v = vector3(sinf(alpha), 0.0f, cosf(alpha)) * radius;

            vector3 n(0.0f, -1.0f, 0.0f); // normal, on base it just points down!

            float u_c = 0.5f + 0.5f * sinf(alpha);
            float v_c = 0.5f + 0.5f * cosf(alpha);

            vector2 t(u_c, v_c);

            size_t center_i = 0;
            size_t current_i = i + 1;
            size_t next_i = (i + 1) % slices + 1;

            vertices.push_back(v);
            normals.push_back(n);
            tex_coords.push_back(t);

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

                // there's a smart explanation for this normal calculation on my notebook i swear
                n.normalize();

                float u_c = (float)j / (float)slices;
                float v_c = 1.0f - (current_height / height);
                vector2 t(u_c, v_c);

                vertices.push_back(v);
                normals.push_back(n);
                tex_coords.push_back(t);

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
        size_t peak_i = vertices.size();

        for (size_t i = 0; i < slices; i++)
        {
            alpha = angle_delta * i;
            float current_radius = radius_delta * (1);

            size_t top_ring = index_base + (stacks - 1) * slices + i;
            size_t next_top = index_base + (stacks - 1) * slices + (i + 1) % slices;

            // Compute the position of the peak (same position for all, but different vertex!)
            vector3 peak(0.0f, height, 0.0f);

            // Compute normal based on triangle face (flat-shaded)
            vector3 n(sinf(alpha) * current_radius * hypotenuse / height, radius / hypotenuse, cosf(alpha) * current_radius * hypotenuse / height);
            n.normalize();

            vector2 peak_tex_coord(0.5f, 0.0f);

            // Add unique peak vertex and its unique normal
            vertices.push_back(peak);
            normals.push_back(n);
            tex_coords.push_back(peak_tex_coord);

            size_t unique_peak_i = vertices.size() - 1;

            indices.push_back(unique_peak_i);
            indices.push_back(top_ring);
            indices.push_back(next_top);
        }
     */

    std::vector<vector3> vertices;
    std::vector<size_t> indices;

    std::vector<vector3> normals;
    std::vector<vector2> tex_coords;

    float angle_delta = (float)(2 * M_PI / slices);
    float alpha = 0;

    // base
    vector3 center(0.0f, 0.0f, 0.0f);
    vector3 center_normal(0.0f, -1.0f, 0.0f);
    vector2 center_tex_coord(0.5f, 0.5f);

    vertices.push_back(center);
    normals.push_back(center_normal);
    tex_coords.push_back(center_tex_coord);

    for (size_t i = 0; i < slices; i++)
    {
        alpha = angle_delta * i;

        vector3 v = vector3(sinf(alpha), 0.0f, cosf(alpha)) * radius;

        vector3 n(0.0f, -1.0f, 0.0f);

        float u_c = 0.5f + 0.5f * sinf(alpha);
        float v_c = 0.5f + 0.5f * cosf(alpha);
        vector2 t(u_c, v_c);

        size_t center_i = 0;
        size_t current_i = i + 1;
        size_t next_i = (i + 1) % slices + 1;

        vertices.push_back(v);
        normals.push_back(n);
        tex_coords.push_back(t);

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
        for (size_t j = 0; j <= slices; j++)
        {
            // NOTE: j <= slices to duplicate first slice at end, which needed to hide texture seam!
            alpha = angle_delta * (j % slices);
            float current_height = height_delta * i;
            float current_radius = radius_delta * (stacks - i);

            vector3 v(sinf(alpha) * current_radius, current_height, cosf(alpha) * current_radius);

            vector3 n = v;
            n.x /= (height / hypotenuse);
            n.y = radius / hypotenuse;
            n.z /= (height / hypotenuse);
            n.normalize();

            float u_c = (float)j / (float)slices;
            float v_c = 1.0f - (current_height / height);
            vector2 t(u_c, v_c);

            vertices.push_back(v);
            normals.push_back(n);
            tex_coords.push_back(t);

            if (i == 0)
                continue;

            size_t this_current_stack = index_base + i * (slices + 1) + j;
            size_t this_prev_stack = index_base + (i - 1) * (slices + 1) + j;
            size_t next_current_stack = index_base + i * (slices + 1) + (j + 1);
            size_t next_prev_stack = index_base + (i - 1) * (slices + 1) + (j + 1);

            if (j < slices)
            {
                indices.push_back(this_current_stack);
                indices.push_back(this_prev_stack);
                indices.push_back(next_prev_stack);

                indices.push_back(this_current_stack);
                indices.push_back(next_prev_stack);
                indices.push_back(next_current_stack);
            }
        }
    }

    // peak
    size_t peak_i = vertices.size();

    for (size_t i = 0; i < slices; i++)
    {
        alpha = angle_delta * i;
        float current_radius = radius_delta * (1);

        size_t top_ring = index_base + (stacks - 1) * (slices + 1) + i;
        size_t next_top = index_base + (stacks - 1) * (slices + 1) + (i + 1);

        vector3 peak(0.0f, height, 0.0f);

        vector3 n(sinf(alpha) * current_radius * hypotenuse / height, radius / hypotenuse, cosf(alpha) * current_radius * hypotenuse / height);
        n.normalize();

        vector2 peak_tex_coord(0.5f, 0.0f);

        vertices.push_back(peak);
        normals.push_back(n);
        tex_coords.push_back(peak_tex_coord);

        size_t unique_peak_i = vertices.size() - 1;

        indices.push_back(unique_peak_i);
        indices.push_back(top_ring);
        indices.push_back(next_top);
    }

    // write to file

    file << "111\n";

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

    // normals
    for (size_t j = 0; j < normals.size(); j++)
    {
        if (j != 0)
            file << ";";
        file << normals.at(j);
    }
    file << "\n";

    // texture coordinates
    for (size_t j = 0; j < tex_coords.size(); j++)
    {
        if (j != 0)
            file << ";";
        file << tex_coords.at(j);
    }

    file << std::flush;

    file.close();
}