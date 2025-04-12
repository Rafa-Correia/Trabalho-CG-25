#include "generator/patch.hpp"

void patch_generator::generate(int argc, char **argv)
{
    std::stringstream ss; // used for errors / exceptions

    if (argc != 5)
    {
        throw InvalidArgumentsException("Wrong number of arguments!");
    }

    std::string patch_filepath(argv[2]);
    std::string output_filepath(argv[4]);

    if (!validate_filepath(patch_filepath, ".patch"))
    {
        ss << "Input patch file " << patch_filepath << " is not a .patch file!";
        throw InvalidArgumentsException(ss.str());
    }

    if (!validate_filepath(output_filepath))
    {
        ss << "Output file " << output_filepath << " is not a valid .3d file!";
        throw InvalidArgumentsException(ss.str());
    }

    int tesselation_level;
    try
    {
        tesselation_level = std::stoi(argv[3]);
    }
    catch (const std::exception &)
    {
        throw InvalidArgumentsException("Tesselation level must be a valid integer!");
    }

    if (tesselation_level <= 0)
        throw InvalidArgumentsException("Tesselation level must be larger than 0!");

    // if the file isn't valid the function will throw an exception that will be caught by main, so it's fine.
    parse_file(patch_filepath);

    // todo:
    // generate surface!
    std::vector<vector3> vertices;
    std::vector<size_t> mesh_indices;

    std::vector<vector3> normals; // unused for now
    // std::vector<vector2> tex_coords; //unused and vector2 undefined

    size_t PATCH_COUNT = this->patch_indices.size();
    size_t CONTROL_POINT_COUNT = this->control_points.size();

    float u, v;
    float delta = 1.0f / (tesselation_level);

    size_t base_point_count = 0;
    size_t patch_point_count;

    for (size_t i = 0; i < PATCH_COUNT; i++)
    {
        u = 0;
        v = 0;
        patch_point_count = 0;

        std::vector<int> indices = this->patch_indices.at(i);
        std::vector<vector3> patch_control_points;
        for (size_t j = 0; j < indices.size() /*this will always be 16*/; j++)
        {
            vector3 point = control_points.at(indices.at(j));
            patch_control_points.push_back(point);
        }

        for (size_t j = 0; j <= tesselation_level /*this will always be 16*/; j++)
        {
            u = j * delta;

            vector3 p0 = math_utils::point_on_bezier(u, patch_control_points.at(0 * 4), patch_control_points.at(0 * 4 + 1), patch_control_points.at(0 * 4 + 2), patch_control_points.at(0 * 4 + 3));
            vector3 p1 = math_utils::point_on_bezier(u, patch_control_points.at(1 * 4), patch_control_points.at(1 * 4 + 1), patch_control_points.at(1 * 4 + 2), patch_control_points.at(1 * 4 + 3));
            vector3 p2 = math_utils::point_on_bezier(u, patch_control_points.at(2 * 4), patch_control_points.at(2 * 4 + 1), patch_control_points.at(2 * 4 + 2), patch_control_points.at(2 * 4 + 3));
            vector3 p3 = math_utils::point_on_bezier(u, patch_control_points.at(3 * 4), patch_control_points.at(3 * 4 + 1), patch_control_points.at(3 * 4 + 2), patch_control_points.at(3 * 4 + 3));

            for (size_t k = 0; k <= tesselation_level; k++, patch_point_count++)
            {
                // ss << "i: " << i << " || j: " << j << " || k: " << k;
                // printer::print_info(ss.str());

                v = k * delta;

                vector3 p = math_utils::point_on_bezier(v, p0, p1, p2, p3);
                vertices.push_back(p);

                // needs to connect to previous points!
                // still don't know how to do that

                // dont connect to previous line if you are on the first line (there's no previous line)
                if (j > 0 && k > 0)
                {
                    size_t a = base_point_count + (j - 1) * (tesselation_level + 1) + (k - 1);
                    size_t b = base_point_count + (j - 1) * (tesselation_level + 1) + k;
                    size_t c = base_point_count + j * (tesselation_level + 1) + (k - 1);
                    size_t d = base_point_count + j * (tesselation_level + 1) + k;

                    mesh_indices.push_back(c);
                    mesh_indices.push_back(b);
                    mesh_indices.push_back(a);

                    mesh_indices.push_back(c);
                    mesh_indices.push_back(d);
                    mesh_indices.push_back(b);
                }
            }
        }

        base_point_count += patch_point_count;
    }

    // write to file
    std::ofstream output_file(output_filepath);

    if (!output_file.is_open())
    {
        ss << "Failed to open output file at " << output_filepath << "!";
        throw InvalidArgumentsException(ss.str());
    }

    // by this point the file should be open!
    output_file << "100\n";

    output_file << "0;0;0;" << 10.0f /*this is the bounding sphere radius, i'm not calculating it just yet*/ << "\n";

    for (size_t i = 0; i < vertices.size(); i++)
    {
        if (i != 0)
            output_file << ";";
        output_file << vertices.at(i);
    }
    output_file << "\n";

    for (size_t i = 0; i < mesh_indices.size(); i++)
    {
        if (i != 0)
            output_file << ";";
        output_file << mesh_indices.at(i);
    }

    output_file << std::flush;
    output_file.close();
}

void patch_generator::parse_file(const std::string &filepath)
{
    std::stringstream ss;
    int current_line = 1;

    std::ifstream file(filepath);
    std::string line;

    int n_patches, n_points;

    if (!file.is_open())
    {
        ss << "Could not open " << filepath << " patch file!";
        throw InvalidArgumentsException(ss.str());
    }

    // actually parse it
    if (std::getline(file, line))
    {
        // first line contains number of patches.
        try
        {
            n_patches = std::stoi(line);
        }
        catch (const std::exception &)
        {
            ss << "Number of patches is not a valid integer! (line " << current_line << ")";
            throw InvalidArgumentsException(ss.str());
        }

        if (n_patches <= 0)
        {
            ss << "Number of patches must be larger than 0! (line " << current_line << ")";
        }
    }
    else
        throw InvalidArgumentsException("Couldn't even read the first line! Are you sure it's the correct file?");

    current_line++;

    // this loop will read all patch indices.
    for (int i = 0; i < n_patches; i++, current_line++)
    {
        if (std::getline(file, line))
        {
            std::stringstream line_stream(line);
            std::string token;
            std::vector<int> indices;

            while (std::getline(line_stream, token, ','))
            {
                int index;
                try
                {
                    index = std::stoi(token);
                }
                catch (const std::exception &)
                {
                    ss << "Invalid index on patch " << i << ". All indices must be integers! (line " << current_line << ")";
                    throw InvalidArgumentsException(ss.str());
                }

                indices.push_back(index);
            }

            if (indices.size() != 16)
            {
                ss << "Patch " << i << " doesn't have 16 indices! (line " << current_line << ")";
                throw InvalidArgumentsException(ss.str());
            }

            this->patch_indices.push_back(indices);
        }
        else
        {
            ss << "It seems the file was cut short! Couldn't read all patch indices! Maybe the file is corrupt? (line " << current_line << ")";
            throw InvalidArgumentsException(ss.str());
        }
    }

    // now reading number of points
    if (std::getline(file, line))
    {
        // read n_points
        try
        {
            n_points = std::stoi(line);
        }
        catch (const std::exception &)
        {
            ss << "Number of control points is not a valid integer! (line " << current_line << ")";
            throw InvalidArgumentsException(ss.str());
        }

        if (n_points <= 16)
        {
            ss << "Number of control points needs to be at least 16 to make a patch! (line " << current_line << ")";
            throw InvalidArgumentsException(ss.str());
        }
    }
    else
    {
        ss << "Couldn't read number of points! Maybe the file is corrupt? (line " << current_line << ")";
        throw InvalidArgumentsException(ss.str());
    }
    current_line++;

    // this loop will read all control points
    for (int i = 0; i < n_points; i++, current_line++)
    {
        /* code */
        if (std::getline(file, line))
        {
            std::stringstream line_stream(line);
            std::string token;
            float control_point_coordinates[3];
            for (int j = 0; j < 3; j++)
            {
                if (std::getline(line_stream, token, ','))
                {
                    // something
                    try
                    {
                        control_point_coordinates[j] = std::stof(token);
                    }
                    catch (const std::exception &)
                    {
                        ss << "Control point " << i << " has non float coordinate! (line " << current_line << ")";
                        throw InvalidArgumentsException(ss.str());
                    }
                }
                else
                {
                    ss << "Control point " << i << " is missing one or more coordinates! (line " << current_line << ")";
                    throw InvalidArgumentsException(ss.str());
                }
            }

            vector3 point(control_point_coordinates[0], control_point_coordinates[1], control_point_coordinates[2]);
            this->control_points.push_back(point);
        }
        else
        {
            ss << "It seems the file was cut short! Couldn't read all control points! Maybe the file is corrupt? (line " << current_line << ")";
            throw InvalidArgumentsException(ss.str());
        }
    }

    // printer::print_info("Patch file loaded correctly!");
}
