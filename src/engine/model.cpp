#include "engine/model.hpp"

model::model(tinyxml2::XMLElement *root)
{
    parse_model(root);
}

void model::render_model(frustum &view_frustum, bool frustum_cull, vector3 &position, bool render_bounding_sphere, matrix4x4 &camera_transform)
{
    if (render_bounding_sphere)
    {
#ifdef USE_LIGHTING
        glDisable(GL_LIGHTING);
#endif

        glColor3f(1.0f, 0.0f, 0.0f);
        glPushMatrix();

        glLoadIdentity();
        glMultMatrixf(camera_transform);
        glTranslatef(position.x, position.y, position.z);
        glTranslatef(bounding_sphere.x, bounding_sphere.y, bounding_sphere.z);
        glutWireSphere(bounding_sphere.w, 10, 10);

        glPopMatrix();

#ifdef USE_LIGHTING
        glEnable(GL_LIGHTING);
#endif
    }

#ifndef IGNORE_FRUSTUM_CULL
    if (frustum_cull && !view_frustum.inside_frustum(position, bounding_sphere.w))
        return;
#endif

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexPointer(3, GL_FLOAT, 0, 0);

#ifndef USE_LIGHTING
    glColor3f(this->mat.diffuse.x, this->mat.diffuse.y, this->mat.diffuse.z);
#endif

    if (has_normals)
    {
#ifdef USE_LIGHTING
        this->mat.apply_material();
        glBindBuffer(GL_ARRAY_BUFFER, NORMAL_BUFFER);
        glNormalPointer(GL_FLOAT, 0, 0);
#endif
    }

    if (!has_ebo)
    {
        glDrawArrays(GL_TRIANGLES, 0, object_count);
    }
    else
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glDrawElements(GL_TRIANGLES, object_count, GL_UNSIGNED_INT, NULL);
    }
    return;
}

void model::parse_model(tinyxml2::XMLElement *root)
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

    parse_file(filepath);

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

void model::parse_file(const std::string &filepath)
{
    std::stringstream ss;
    std::ifstream file(filepath);

    if (!file.is_open())
    {
        ss << "Failed to open file: " << filepath;
        printer::print_exception(ss.str(), "model file_parse");
        throw FailedToParseModelException("");
    }

    bool t_flag = false;

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
            else
                has_ebo = false;

            if (line_data[1] == '1')
            {
                has_normals = true;
                data_order.push_back('n');
#ifndef USE_LIGHTING
                printer::print_warning("Normals are loaded but USE_LIGHTING is not defined. This may cause some problems. Define USE_LIGTHING in model.hpp.");
#endif
            }
            else
                has_normals = false;

            if (line_data[2] == '1')
            {
                t_flag = true;
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

            this->bounding_sphere = vector4(bounding_sphere_info_vector.at(0), bounding_sphere_info_vector.at(1), bounding_sphere_info_vector.at(2), bounding_sphere_info_vector.at(3));
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

#ifndef USE_LIGHTING
                printer::print_info("USE_LIGHTING is not defined but normals are loaded. Define USE_LIGHTING in group.hpp to enable lighting.\n\n", "debug");
#endif
            }
            else if (data_order.at(line_index - 3) == 't')
            {
                // read texture coordinates
                // unused for now
            }
        }

        line_index++;
    }

    file.close();
}