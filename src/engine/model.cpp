#include "engine/model.hpp"

model::model(tinyxml2::XMLElement *root, float bound_scale_factor)
{
    parse_model(root, bound_scale_factor);
}

void model::render_model(frustum &view_frustum, bool frustum_cull, vector3 &position, bool render_bounding_sphere, matrix4x4 &camera_transform)
{
    if (render_bounding_sphere)
    {
        glDisable(GL_LIGHTING);

        glPushMatrix();

        glColor3f(1.0f, 0.0f, 0.0f);

        glLoadIdentity();
        glMultMatrixf(camera_transform);
        glTranslatef(position.x, position.y, position.z);
        glTranslatef(bounding_sphere.x, bounding_sphere.y, bounding_sphere.z);
        glutWireSphere(bounding_sphere.w, 10, 10);

        glPopMatrix();

        glEnable(GL_LIGHTING);
    }

#ifndef IGNORE_FRUSTUM_CULL
    if (frustum_cull && !view_frustum.inside_frustum(position, bounding_sphere.w))
        return;
#endif

    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    glVertexPointer(3, GL_FLOAT, 0, 0);

    if (this->has_normals)
    {
        this->mat.apply_material();
        glBindBuffer(GL_ARRAY_BUFFER, this->NORMAL_BUFFER);
        glNormalPointer(GL_FLOAT, 0, 0);
    }

    if (this->has_texture_coordinates)
    {
        glEnable(GL_TEXTURE_2D);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);

        glBindTexture(GL_TEXTURE_2D, this->TEXTURE);

        glBindBuffer(GL_ARRAY_BUFFER, this->TEXTURE_COORDINATE_BUFFER);
        glTexCoordPointer(2, GL_FLOAT, 0, 0);
    }
    else
    {
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        glDisable(GL_TEXTURE_2D);
    }

    if (!this->has_ebo)
    {
        glDrawArrays(GL_TRIANGLES, 0, this->object_count);
    }
    else
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
        glDrawElements(GL_TRIANGLES, this->object_count, GL_UNSIGNED_INT, 0);
    }

    if (this->has_texture_coordinates)
    {
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    return;
}

void model::parse_model(tinyxml2::XMLElement *root, float bound_scale_factor)
{
    std::stringstream ss;

    const char *filepath;
    tinyxml2::XMLError file_result = root->QueryStringAttribute("file", &filepath);
    if (file_result != tinyxml2::XML_SUCCESS)
    {
        ss << "file attribute of model element is either missing or not a valid string!";
        printer::print_exception(ss.str());
        throw FailedToParseModelException("");
    }

    parse_file(filepath, bound_scale_factor);

    tinyxml2::XMLElement *texture = root->FirstChildElement("texture");
    if (texture)
    {
        const char *tex_filepath;
        tinyxml2::XMLError tex_result = texture->QueryStringAttribute("file", &tex_filepath);
        if (tex_result != tinyxml2::XML_SUCCESS)
        {
            if (this->has_texture_coordinates)
            {
                printer::print_exception("Model has texture coordinates but no valid texture filepath.");
                throw FailedToParseModelException("");
            }
        }
        else
        {
            if (this->has_texture_coordinates)
                this->load_texture(tex_filepath);
            else
            {
                printer::print_warning("Valid texture filepath was found but model doesn't have texture coordinates. The texture will not be loaded.");
            }
        }
    }
    else
    {
        if (this->has_texture_coordinates)
        {
            printer::print_warning("Model's file has texture coordinates but no texture element was found. The 'has_texture_coordinates' flag is no longer set for this model.");
            this->has_texture_coordinates = false;
        }
    }

    if (!this->has_texture_coordinates)
        this->TEXTURE = 0;

    tinyxml2::XMLElement *color = root->FirstChildElement("color");
    if (color)
    {
        vector3 diffuse_v;
        vector3 ambient_v;
        vector3 specular_v;
        vector3 emissive_v;
        float shininess_v;

        tinyxml2::XMLElement *diffuse_element = color->FirstChildElement("diffuse");
        if (diffuse_element)
        {
            float r, g, b;
            if (diffuse_element->QueryFloatAttribute("R", &r) != tinyxml2::XML_SUCCESS || diffuse_element->QueryFloatAttribute("G", &g) != tinyxml2::XML_SUCCESS || diffuse_element->QueryFloatAttribute("B", &b) != tinyxml2::XML_SUCCESS)
            {
                ss << "R, G or B attribute of diffuse element is either missing or not a valid float!";
                printer::print_exception(ss.str(), "parse_model");
                throw FailedToParseModelException("");
            }

            diffuse_v = vector3(r, g, b);
        }
        else
        {
            ss << "diffuse element of color element is missing!";
            printer::print_exception(ss.str());
            throw FailedToParseModelException("");
        }

        tinyxml2::XMLElement *ambient_element = color->FirstChildElement("ambient");
        if (ambient_element)
        {
            float r, g, b;
            if (ambient_element->QueryFloatAttribute("R", &r) != tinyxml2::XML_SUCCESS || ambient_element->QueryFloatAttribute("G", &g) != tinyxml2::XML_SUCCESS || ambient_element->QueryFloatAttribute("B", &b) != tinyxml2::XML_SUCCESS)
            {
                ss << "R, G or B attribute of ambient element is either missing or not a valid float!";
                printer::print_exception(ss.str(), "parse_model");
                throw FailedToParseModelException("");
            }

            ambient_v = vector3(r, g, b);
        }
        else
        {
            ss << "ambient element of color element is missing!";
            printer::print_exception(ss.str());
            throw FailedToParseModelException("");
        }

        tinyxml2::XMLElement *specular_element = color->FirstChildElement("specular");
        if (specular_element)
        {
            float r, g, b;
            if (specular_element->QueryFloatAttribute("R", &r) != tinyxml2::XML_SUCCESS || specular_element->QueryFloatAttribute("G", &g) != tinyxml2::XML_SUCCESS || specular_element->QueryFloatAttribute("B", &b) != tinyxml2::XML_SUCCESS)
            {
                ss << "R, G or B attribute of specular element is either missing or not a valid float!";
                printer::print_exception(ss.str(), "parse_model");
                throw FailedToParseModelException("");
            }

            specular_v = vector3(r, g, b);
        }
        else
        {
            ss << "specular element of color element is missing!";
            printer::print_exception(ss.str());
            throw FailedToParseModelException("");
        }

        tinyxml2::XMLElement *emissive_element = color->FirstChildElement("emissive");
        if (emissive_element)
        {
            float r, g, b;
            if (emissive_element->QueryFloatAttribute("R", &r) != tinyxml2::XML_SUCCESS || emissive_element->QueryFloatAttribute("G", &g) != tinyxml2::XML_SUCCESS || emissive_element->QueryFloatAttribute("B", &b) != tinyxml2::XML_SUCCESS)
            {
                ss << "R, G or B attribute of emissive element is either missing or not a valid float!";
                printer::print_exception(ss.str(), "parse_model");
                throw FailedToParseModelException("");
            }

            emissive_v = vector3(r, g, b);
        }
        else
        {
            ss << "emissive element of color element is missing!";
            printer::print_exception(ss.str());
            throw FailedToParseModelException("");
        }

        tinyxml2::XMLElement *shininess_element = color->FirstChildElement("shininess");
        if (shininess_element)
        {
            if (shininess_element->QueryFloatAttribute("value", &shininess_v) != tinyxml2::XML_SUCCESS)
            {
                ss << "value attribute of emissive element is either missing or not a valid float!";
                printer::print_exception(ss.str(), "parse_model");
                throw FailedToParseModelException("");
            }
        }
        else
        {
            ss << "shininess element of color element is missing!";
            printer::print_exception(ss.str());
            throw FailedToParseModelException("");
        }

        this->mat = material(diffuse_v, ambient_v, specular_v, emissive_v, shininess_v);
    }
    else
    {
        printer::print_warning("color element of model wasn't found. Assuming default values...\n");
        this->mat = material();
    }
}

void model::parse_file(const std::string &filepath, float bound_scale_factor)
{
    std::stringstream ss;
    std::ifstream file(filepath);

    if (!file.is_open())
    {
        ss << "Failed to open file: " << filepath;
        printer::print_exception(ss.str(), "model file_parse");
        throw FailedToParseModelException("");
    }

    std::vector<float> vertices;
    std::vector<int> indices;
    std::vector<float> normals;
    std::vector<float> tex_coords;

    std::string line;
    std::vector<char> data_order;
    int line_index = 0;
    while (std::getline(file, line))
    {
        std::stringstream ss(line);
        std::string token;

        if (line_index == 0)
        {
            const char *line_data = line.data();
            if (line_data[0] == '1')
            {
                has_ebo = true;
                data_order.push_back('i');
            }

            if (line_data[1] == '1')
            {
                has_normals = true;
                data_order.push_back('n');
            }

            if (line_data[2] == '1')
            {
                has_texture_coordinates = true;
                data_order.push_back('t');
            }
        }
        else if (line_index == 1)
        {
            std::vector<float> bounding_sphere_info_vector;
            while (std::getline(ss, token, ';'))
            {
                float bounding_info_token = std::stof(token);
                bounding_sphere_info_vector.push_back(bounding_info_token);
            }

            this->bounding_sphere = vector4(bounding_sphere_info_vector.at(0), bounding_sphere_info_vector.at(1), bounding_sphere_info_vector.at(2), bounding_sphere_info_vector.at(3) * bound_scale_factor);
        }
        else if (line_index == 2)
        { // vertices
            while (std::getline(ss, token, ';'))
            {
                float vertex_float = std::stof(token);
                vertices.push_back(vertex_float);
            }

            glGenBuffers(1, &VBO);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

            if (!has_ebo)
            {
                object_count = vertices.size() / 3;
            }
        }
        else
        { // all the others
            if (data_order.size() == 0)
                break; // only indices

            if (line_index - 3 > data_order.size())
            {
                printer::print_warning("Unexpected data at the end of file. Perhaps a flag isn't set? Ignoring extra data... \n");
                break;
            }

            if (data_order.at(line_index - 3) == 'i')
            {
                // read indices
                while (std::getline(ss, token, ';'))
                {
                    int index = std::stoi(token);
                    indices.push_back(index);
                }

                glGenBuffers(1, &EBO);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * indices.size(), indices.data(), GL_STATIC_DRAW);

                object_count = indices.size();
            }
            else if (data_order.at(line_index - 3) == 'n')
            {
                // read normals
                // unused for now
                while (std::getline(ss, token, ';'))
                {
                    float normal_float = std::stof(token);
                    normals.push_back(normal_float);
                }

                glGenBuffers(1, &NORMAL_BUFFER);
                glBindBuffer(GL_ARRAY_BUFFER, NORMAL_BUFFER);
                glBufferData(GL_ARRAY_BUFFER, sizeof(float) * normals.size(), normals.data(), GL_STATIC_DRAW);
            }
            else if (data_order.at(line_index - 3) == 't')
            {
                // read texture coordinates
                while (std::getline(ss, token, ';'))
                {
                    float tex_float = std::stof(token);
                    tex_coords.push_back(tex_float);
                }

                glGenBuffers(1, &TEXTURE_COORDINATE_BUFFER);
                glBindBuffer(GL_ARRAY_BUFFER, TEXTURE_COORDINATE_BUFFER);
                glBufferData(GL_ARRAY_BUFFER, tex_coords.size() * sizeof(float), tex_coords.data(), GL_STATIC_DRAW);
            }
        }

        line_index++;
    }

    file.close();
}

void model::load_texture(const std::string &filepath)
{
    // std::stringstream ss;
    // ss << "load_texture has been called (" << filepath << ")...";
    // printer::print_info(ss.str());

    // ss.clear();
    // ss.str(std::string());

    // ss << "Image path converted: " << (ILstring)filepath.data();
    // printer::print_info(ss.str());

    unsigned int tw, th;
    unsigned char *tex_data;
    ILuint t;
    ilGenImages(1, &t);
    ilBindImage(t);

    // printer::print_info("Trying to load image...");
    ilLoadImage((ILstring)filepath.data());
    // printer::print_info("Done!");

    tw = ilGetInteger(IL_IMAGE_WIDTH);
    th = ilGetInteger(IL_IMAGE_HEIGHT);

    // ss.clear();
    // ss.str(std::string());

    // ss << "tw: " << tw << " || th: " << th;
    // printer::print_info(ss.str());

    ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
    tex_data = ilGetData();

    glGenTextures(1, &this->TEXTURE);

    glBindTexture(GL_TEXTURE_2D, this->TEXTURE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tw, th, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex_data);
    glGenerateMipmap(GL_TEXTURE_2D);
}
