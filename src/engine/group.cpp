#include "engine/group.hpp"

group::group(tinyxml2::XMLElement *root, float parent_scale)
{
    model_matrix = matrix4x4::Identity();
    group::parse_group(root, parent_scale);
}

// render / update

void group::render_group(matrix4x4 &camera_transform, frustum &view_frustum, bool frustum_cull, bool render_bounding_spheres, bool draw_translation_path)
{
    glPushMatrix();
    glMultMatrixf(model_matrix);

    for (size_t i = 0; i < models.size(); i++)
    {
        model mod = models.at(i);
        mod.render_model(view_frustum, frustum_cull, this->position, render_bounding_spheres, camera_transform);
    }

    for (size_t i = 0; i < sub_groups.size(); i++)
    {
        sub_groups.at(i).render_group(camera_transform, view_frustum, frustum_cull, render_bounding_spheres, draw_translation_path);
    }

    glPopMatrix();

    if (draw_translation_path)
    {
#ifdef USE_LIGHTING
        glDisable(GL_LIGHTING);
#endif
        if (t)
            t->draw_path();
#ifdef USE_LIGHTING
        glEnable(GL_LIGHTING);
#endif
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
            model_matrix = model_matrix * *t; // dereference to be able to cast to matrix4x4
        if (transform_order[i] == 'r')
            model_matrix = model_matrix * *r; // dereference again (look above)
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
                    printer::print_exception(ss.str(), "group::parse_group");
                    throw FailedToParseGroupException(ss.str());
                }
                seen.insert(tag);
            }
            else
            {
                ss << "Invalid element inside transform: " << tag;
                printer::print_exception(ss.str(), "group::parse_group");
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
                    tinyxml2::XMLError time_result = child->QueryFloatAttribute("time", &time);
                    if (time_result != tinyxml2::XML_SUCCESS)
                    {
                        // only valid error is XML_WRONG_ATTRIBUTE_TYPE
                        //  only this error can happen, since we checked if existed before
                        ss << "\"time\" attribute in translate element is not a valid float!";
                        printer::print_exception(ss.str(), "group::parse_group");
                        throw FailedToParseGroupException(ss.str());
                    }

                    // since we checked if it existed and was a valid float, we only need to check if larger than 0
                    if (time <= 0)
                    {
                        ss << "\"time\" attribute in translate element must be larger than 0!";
                        printer::print_exception(ss.str(), "group::parse_group");
                        throw FailedToParseGroupException(ss.str());
                    }

                    // dont forget to read boolean "align". it is not being done yet
                    bool align = false;
                    tinyxml2::XMLError align_result = child->QueryBoolAttribute("align", &align);
                    if (align_result != tinyxml2::XML_SUCCESS)
                    {
                        printer::print_warning("Couldn't read align attribute! Defaulting to false...");
                        align = false;
                    }

                    // optional loop boolean.
                    bool loop = true;
                    tinyxml2::XMLError loop_result = child->QueryBoolAttribute("loop", &loop);
                    if (loop_result != tinyxml2::XML_SUCCESS)
                    {
                        printer::print_info("No loop attribute found. Defaulting to true...");
                        loop = true;
                    }

                    std::vector<vector3> points;
                    tinyxml2::XMLElement *point = child->FirstChildElement();
                    while (point)
                    {
                        float x, y, z;
                        if (point->QueryFloatAttribute("x", &x) != tinyxml2::XML_SUCCESS || point->QueryFloatAttribute("y", &y) != tinyxml2::XML_SUCCESS || point->QueryFloatAttribute("z", &z) != tinyxml2::XML_SUCCESS)
                        {
                            // load all points, if point has invalid / non existant attributes then you're isnide this block~
                            ss << "A \"point\" element has invalid or missing attributes!";
                            printer::print_exception(ss.str(), "group::parse_group");
                            throw FailedToParseGroupException(ss.str());
                        }
                        vector3 p_pos = vector3(x, y, z);
                        points.push_back(p_pos);

                        point = point->NextSiblingElement();
                    }

                    if (points.size() < 4)
                    {
                        ss << "Minimum number of \"point\" elements inside translation is 4!";
                        printer::print_exception(ss.str(), "group::parse_group");
                        throw FailedToParseGroupException(ss.str());
                    }

                    this->t = new translation_dynamic(time, align, points, loop);
                }

                else
                {

                    float x = 0, y = 0, z = 0;
                    if (child->QueryFloatAttribute("x", &x) != tinyxml2::XML_SUCCESS || child->QueryFloatAttribute("y", &y) != tinyxml2::XML_SUCCESS || child->QueryFloatAttribute("z", &z) != tinyxml2::XML_SUCCESS)
                    {
                        ss << "x, y or z attribute in translate element is either missing or not a valid float!";
                        printer::print_exception(ss.str(), "group::parse_group");
                        throw FailedToParseGroupException(ss.str());
                    }

                    this->t = new translation_static(vector3(x, y, z));
                }
            }
            else if (tag == "rotate")
            {
                this->transform_order[current_transform] = 'r';
                current_transform++;

                const char *time_value = child->Attribute("time");
                if (time_value)
                {
                    // since there's a time attribute, it means it's a dynamic rotation
                    float time;
                    tinyxml2::XMLError result = child->QueryFloatAttribute("time", &time);
                    if (result != tinyxml2::XML_SUCCESS)
                    {
                        ss << "\"time\" attribute in rotate element is not a valid float!";
                        printer::print_exception(ss.str(), "group::parse_group");
                        throw FailedToParseGroupException(ss.str());
                    }

                    if (time <= 0)
                    {
                        ss << "\"time\" attribute in rotate element must be larger than 0!";
                        printer::print_exception(ss.str(), "group::parse_group");
                        throw FailedToParseGroupException(ss.str());
                    }

                    float x, y, z;

                    if (child->QueryFloatAttribute("x", &x) != tinyxml2::XML_SUCCESS || child->QueryFloatAttribute("y", &y) != tinyxml2::XML_SUCCESS || child->QueryFloatAttribute("z", &z) != tinyxml2::XML_SUCCESS)
                    {
                        ss << "x, y or z attribute of rotate element is either missing or not a valid float!";
                        printer::print_exception(ss.str(), "group::parse_group");
                        throw FailedToParseGroupException(ss.str());
                    }

                    this->r = new rotation_dynamic(time, vector3(x, y, z));
                }
                else
                {
                    float angle = 0, x = 0, y = 0, z = 0;

                    if (child->QueryFloatAttribute("angle", &angle) != tinyxml2::XML_SUCCESS)
                    {
                        ss << "angle attribute of rotate element is either missing or not a valid float! Perhaps you're missing a \"time\" attribute?";
                        printer::print_exception(ss.str(), "group::parse_group");
                        throw FailedToParseGroupException(ss.str());
                    }

                    if (child->QueryFloatAttribute("x", &x) != tinyxml2::XML_SUCCESS || child->QueryFloatAttribute("y", &y) != tinyxml2::XML_SUCCESS || child->QueryFloatAttribute("z", &z) != tinyxml2::XML_SUCCESS)
                    {
                        ss << "x, y or z attribute of rotate element is either missing or not a valid float!";
                        printer::print_exception(ss.str(), "group::parse_group");
                        throw FailedToParseGroupException(ss.str());
                    }

                    this->r = new rotation_static(angle, vector3(x, y, z));
                }
            }
            else if (tag == "scale")
            {
                // transform_order.push_back('s');
                this->transform_order[current_transform] = 's';
                current_transform++;

                float x = 1, y = 1, z = 1;

                if (child->QueryFloatAttribute("x", &x) != tinyxml2::XML_SUCCESS || child->QueryFloatAttribute("y", &y) != tinyxml2::XML_SUCCESS || child->QueryFloatAttribute("z", &z) != tinyxml2::XML_SUCCESS)
                {
                    ss << "x, y or z attribute of rotate element is either missing or not a valid float!";
                    printer::print_exception(ss.str(), "group::parse_group");
                    throw FailedToParseGroupException(ss.str());
                }

                if (x >= y && x >= z)
                    bound_scaling *= x;
                else if (y >= z && y >= z)
                    bound_scaling *= y;
                else if (z >= y && z >= x)
                    bound_scaling *= z;

                this->s = matrix4x4::Scale(vector3(x, y, z));
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
        tinyxml2::XMLElement *model_element = models->FirstChildElement("model");
        while (model_element)
        {
            loaded_model_at_least_once = true;
            this->models.push_back(model(model_element));
            model_element = model_element->NextSiblingElement("model");
        }
        if (!loaded_model_at_least_once)
        {
            ss << "A models element must have at least one model child element!";
            printer::print_exception(ss.str(), "group::parse_group");
            throw FailedToParseGroupException(ss.str());
        }
    }
    else
    {
        // std::cout << "A group element must have models child element!" << std::endl;
        // return false;
    }

    // need to parse material!
    // for now use default

    // loop through all subgroups

    tinyxml2::XMLElement *subgroup = root->FirstChildElement("group");
    for (subgroup; subgroup; subgroup = subgroup->NextSiblingElement("group"))
    {
        group sub(subgroup, bound_scaling);
        sub_groups.push_back(sub);
    }
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
