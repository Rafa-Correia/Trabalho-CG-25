#include "group.hpp"

group::group(tinyxml2::XMLElement *root, float parent_scale)
{
    model_matrix = matrix4x4::Identity();
    group::parse_group(root, parent_scale);
}

// render / update

void group::render_group(matrix4x4 &camera_transform, frustum &view_frustum, bool frustum_cull, bool render_bounding_spheres, bool draw_translation_path)
{
    glPushMatrix();
    glMultMatrixf(model_matrix.get_data());

    if (render_bounding_spheres)
    {
        glColor3f(1.0f, 0.0f, 0.0f);
        for (size_t i = 0; i < mesh_bounding_spheres.size(); i++)
        {
            glPushMatrix();
            glLoadIdentity();
            glMultMatrixf(camera_transform.get_data());
            glTranslatef(position.x, position.y, position.z);
            vector4 bounding_sphere_info = mesh_bounding_spheres.at(i);
            glTranslatef(bounding_sphere_info.x, bounding_sphere_info.y, bounding_sphere_info.z);
            glutWireSphere(bounding_sphere_info.w, 10, 10);
            glPopMatrix();
        }
    }

    glColor4f(color.x, color.y, color.z, color.w);
    for (unsigned int j = 0; j < mesh_count; j++)
    {
        if (frustum_cull && !view_frustum.inside_frustum(position, mesh_bounding_spheres.at(j).w))
            continue;

        GLuint VBO = group_vbos.at(j);
        std::tuple<bool, GLuint> EBO_t = group_ebos.at(j);
        int obj_count = vertex_or_index_count.at(j);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glVertexPointer(3, GL_FLOAT, 0, 0);

        if (!std::get<0>(EBO_t))
        {
            glDrawArrays(GL_TRIANGLES, 0, obj_count);
        }
        else
        {

            GLuint EBO = std::get<1>(EBO_t);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glDrawElements(GL_TRIANGLES, obj_count, GL_UNSIGNED_INT, NULL);
        }
    }

    for (size_t i = 0; i < sub_groups.size(); i++)
    {
        sub_groups.at(i).render_group(camera_transform, view_frustum, frustum_cull, render_bounding_spheres, draw_translation_path);
    }

    glPopMatrix();

    if (draw_translation_path)
    {
        if (t)
            t->draw_path();
    }
}

void group::update_group(int delta_time_ms, matrix4x4 parent_transform)
{
    // update transforms here!
    if (t)
        t->update(delta_time_ms);
    if (r)
        r->update(delta_time_ms);

    model_matrix = matrix4x4::Identity();
    for (int i = 0; i < 3; i++)
    {
        if (transform_order[i] == 't')
            model_matrix = model_matrix * t->get_translation();
        if (transform_order[i] == 'r')
            model_matrix = model_matrix * r->get_rotation();
        if (transform_order[i] == 's')
            model_matrix = model_matrix * s; // s (scale) is always static
    }

    // update position
    matrix4x4 full_transform = parent_transform * model_matrix;
    position.x = full_transform.get_data_at_point(3, 0);
    position.y = full_transform.get_data_at_point(3, 1);
    position.z = full_transform.get_data_at_point(3, 2);

    for (size_t i = 0; i < sub_groups.size(); i++)
    {
        sub_groups.at(i).update_group(delta_time_ms, full_transform);
    }
}

// parsing / loading

void group::parse_group(tinyxml2::XMLElement *root, float parent_scale)
{
    std::stringstream ss; // used for exceptions
    float bound_scaling = parent_scale;
    int current_transform = 0;
    tinyxml2::XMLElement *transform = root->FirstChildElement("transform");
    if (transform)
    {
        std::unordered_set<std::string> seen;

        for (tinyxml2::XMLElement *child = transform->FirstChildElement(); child; child = child->NextSiblingElement())
        {
            std::string tag = child->Name();

            if (tag == "translate" || tag == "rotate" || tag == "scale")
            {
                if (seen.count(tag))
                {
                    ss << "Duplicate element inside transform: " << tag;
                    throw FailedToParseGroupException(ss.str());
                }
                seen.insert(tag);
            }
            else
            {
                ss << "Invalid element inside transform: " << tag;
                throw FailedToParseGroupException(ss.str());
            }

            if (tag == "translate")
            {
                this->transform_order[current_transform] = 't';
                current_transform++;

                const char *time_value = child->Attribute("time");
                if (time_value)
                {
                    // this now means this is a dynamic translation
                    float time;
                    tinyxml2::XMLError result = child->QueryFloatAttribute("time", &time);
                    if (result == tinyxml2::XML_WRONG_ATTRIBUTE_TYPE)
                    {
                        // only this error can happen, since we checked if existed before
                        ss << "\"time\" attribute in translation element is not a valid float!";
                        throw FailedToParseGroupException(ss.str());
                    }

                    // since we checked if it existed and was a valid float, we only need to check if larger than 0
                    if (time <= 0)
                    {
                        ss << "\"time\" attribute must be larger than 0!";
                        throw FailedToParseGroupException(ss.str());
                    }

                    // dont forget to read boolean "align". it is not being done yet

                    std::vector<vector3> points;
                    tinyxml2::XMLElement *point = child->FirstChildElement();
                    while (point)
                    {
                        float x, y, z;
                        if (point->QueryFloatAttribute("x", &x) != tinyxml2::XML_SUCCESS || point->QueryFloatAttribute("y", &y) != tinyxml2::XML_SUCCESS || point->QueryFloatAttribute("z", &z) != tinyxml2::XML_SUCCESS)
                        {
                            // load all points, if point has invalid / non existant attributes then you're isnide this block~
                            ss << "A \"point\" element has invalid or missing attributes!";
                            throw FailedToParseGroupException(ss.str());
                        }
                        vector3 p_pos = vector3(x, y, z);
                        points.push_back(p_pos);

                        point = point->NextSiblingElement();
                    }

                    if (points.size() < 4)
                    {
                        ss << "Minimum number of \"point\" elements inside translation is 4!";
                        throw FailedToParseGroupException(ss.str());
                    }

                    this->t = new translation_dynamic(time, false, points);
                }

                else
                {
                    float t_x = 0, t_y = 0, t_z = 0;
                    child->QueryFloatAttribute("x", &t_x);
                    child->QueryFloatAttribute("y", &t_y);
                    child->QueryFloatAttribute("z", &t_z);

                    this->t = new translation_static(vector3(t_x, t_y, t_z));
                }
            }
            else if (tag == "rotate")
            {

                float angle = 0, r_x = 0, r_y = 0, r_z = 0;
                child->QueryFloatAttribute("angle", &angle);
                child->QueryFloatAttribute("x", &r_x);
                child->QueryFloatAttribute("y", &r_y);
                child->QueryFloatAttribute("z", &r_z);

                this->r = new rotation_static(angle, vector3(r_x, r_y, r_z));
            }
            else if (tag == "scale")
            {
                // transform_order.push_back('s');
                this->transform_order[current_transform] = 's';
                current_transform++;

                float s_x = 1, s_y = 1, s_z = 1;
                child->QueryFloatAttribute("x", &s_x);
                child->QueryFloatAttribute("y", &s_y);
                child->QueryFloatAttribute("z", &s_z);

                if (s_x >= s_y && s_x >= s_z)
                    bound_scaling *= s_x;
                else if (s_y >= s_z && s_y >= s_z)
                    bound_scaling *= s_y;
                else if (s_z >= s_y && s_z >= s_x)
                    bound_scaling *= s_z;

                this->s = matrix4x4::Scale(vector3(s_x, s_y, s_z));
            }
        }
    }
    else
    {
        // no transform
        // do nothing?
    }

    tinyxml2::XMLElement *models = root->FirstChildElement("models");
    if (models)
    {
        bool loaded_model_at_least_once = false;
        tinyxml2::XMLElement *model = models->FirstChildElement("model");
        while (model)
        {
            loaded_model_at_least_once = true;
            const char *filepath = model->Attribute("file");
            if (filepath)
            {
                if (!parse_model_file(filepath))
                {
                    ss << "Model file is invalid: " << filepath;
                    throw FailedToParseGroupException(ss.str());
                }

                mesh_bounding_spheres.at(mesh_count).w *= bound_scaling;

                mesh_count++;
            }
            else
            {
                ss << "A model element must have a file attribute!";
                throw FailedToParseGroupException(ss.str());
            }
            model = model->NextSiblingElement("model");
        }
        if (!loaded_model_at_least_once)
        {
            ss << "A models element must have at least one model child element!";
            throw FailedToParseGroupException(ss.str());
        }
    }
    else
    {
        // std::cout << "A group element must have models child element!" << std::endl;
        // return false;
    }

    tinyxml2::XMLElement *color_element = root->FirstChildElement("color");
    if (color_element)
    {
        float r = 1.0f, g = 1.0f, b = 1.0f, a = 1.0f;

        color_element->QueryFloatAttribute("r", &r);
        color_element->QueryFloatAttribute("g", &g);
        color_element->QueryFloatAttribute("b", &b);
        color_element->QueryFloatAttribute("a", &a);

        if (a <= 0.0f || a > 1.0f)
            a = 1.0f;

        color = vector4(r, g, b, a);
    }
    else
    {
        // if no color element then mesh is white.
        color = vector4(1.0f, 1.0f, 1.0f, 1.0f);
    }

    // loop through all subgroups

    tinyxml2::XMLElement *subgroup = root->FirstChildElement("group");
    for (subgroup; subgroup; subgroup = subgroup->NextSiblingElement("group"))
    {
        group sub(subgroup, bound_scaling);
        sub_groups.push_back(sub);
    }
}

bool group::parse_model_file(const char *filepath)
{
    std::ifstream file(filepath);

    if (!file)
    {
        std::cout << "Failed to open file: " << filepath << std::endl;
        return false;
    }

    bool i_flag = false, n_flag = false, t_flag = false;

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
                i_flag = true;
                data_order.push_back('i');
            }
            if (line_data[1] == '1')
            {
                n_flag = true;
                data_order.push_back('n');
            }
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

            mesh_bounding_spheres.push_back(vector4(bounding_sphere_info_vector.at(0), bounding_sphere_info_vector.at(1), bounding_sphere_info_vector.at(2), bounding_sphere_info_vector.at(3)));
        }
        else if (line_index == 2)
        { // vertices
            while (std::getline(ss, token, ';'))
            {
                float vertex_float = std::stof(token);
                vertices.push_back(vertex_float);
            }

            GLuint VBO;
            glGenBuffers(1, &VBO);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

            group_vbos.push_back(VBO);

            if (!i_flag)
            {
                group_ebos.push_back(std::tuple<bool, GLuint>(false, 0));
                vertex_or_index_count.push_back(vertices.size() / 3);
            }
            if (!n_flag)
            {
            }

            // mesh_vertices_buffer.push_back(vertices);
        }
        else
        { // all the others
            if (data_order.size() == 0)
                break; // only indices

            if (data_order.at(line_index - 3) == 'i')
            {
                // read indices
                while (std::getline(ss, token, ';'))
                {
                    int index = std::stoi(token);
                    indices.push_back(index);
                }

                GLuint EBO;

                glGenBuffers(1, &EBO);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * indices.size(), indices.data(), GL_STATIC_DRAW);

                std::tuple<bool, GLuint> EBO_tuple(true, EBO);

                group_ebos.push_back(EBO_tuple);

                vertex_or_index_count.push_back(indices.size());
            }
            else if (data_order.at(line_index - 3) == 'n')
            {
                // read normals
                // unused for now
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

    return true;
}

// getters

std::vector<vector3> group::query_group_positions()
{
    std::vector<vector3> lock_pos;
    lock_pos.push_back(position);

    for (size_t i = 0; i < sub_groups.size(); i++)
    {
        std::vector<vector3> c_locks = sub_groups.at(i).query_group_positions();
        lock_pos.insert(lock_pos.end(), c_locks.begin(), c_locks.end());
    }

    return lock_pos;
}
