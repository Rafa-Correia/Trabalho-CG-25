#include "group.hpp"

group::group(tinyxml2::XMLElement *root) {
    if(!group::parse_group(root)) {
        throw FailedToParseGroupException(std::string("Failed to parse group element!"));
    }
}

bool group::parse_group(tinyxml2::XMLElement *root) {
    tinyxml2::XMLElement *transform = root->FirstChildElement("transform");
    if(transform) {

        std::unordered_set<std::string> seen;

        for(tinyxml2::XMLElement *child = transform->FirstChildElement(); child; child = child->NextSiblingElement()) {
            std::string tag = child->Name();

            if(tag == "translate" || tag == "rotate" || tag == "scale") {
                if(seen.count(tag)) {
                    std::cout << "Duplicate element inside transform: " << tag << std::endl;
                    return false;
                }
                seen.insert(tag);
            }
            else {
                std::cout << "Invalid element inside transform: " << tag << std::endl;
                return false;
            }

            if(tag == "translate") {
                transform_order.push_back('t');

                float t_x = 0, t_y = 0, t_z = 0;
                child->QueryFloatAttribute("x", &t_x);
                child->QueryFloatAttribute("y", &t_y);
                child->QueryFloatAttribute("z", &t_z);

                translate_x = t_x;
                translate_y = t_y;
                translate_z = t_z;
            }
            else if(tag == "rotate") {
                transform_order.push_back('r');

                float angle = 0, r_x = 0, r_y = 0, r_z = 0;
                child->QueryFloatAttribute("angle", &angle);
                child->QueryFloatAttribute("x", &r_x);
                child->QueryFloatAttribute("y", &r_y);
                child->QueryFloatAttribute("z", &r_z);
                
                rotate_angle = angle;
                rotate_x = r_x;
                rotate_y = r_y;
                rotate_z = r_z;
            } 
            else if(tag == "scale") {
                transform_order.push_back('s');

                float s_x = 1, s_y = 1, s_z = 1;
                child->QueryFloatAttribute("x", &s_x);
                child->QueryFloatAttribute("y", &s_y);
                child->QueryFloatAttribute("z", &s_z);

                scale_x = s_x;
                scale_y = s_y;
                scale_z = s_z;
            }
        }

    }
    else {
        //no transform
        //do nothing?
    }


    tinyxml2::XMLElement *models = root->FirstChildElement("models");
	if(models) {
		bool loaded_model_at_least_once = false;
		tinyxml2::XMLElement *model = models->FirstChildElement("model");
		while(model) {
			loaded_model_at_least_once = true;
			const char* filepath = model->Attribute("file");
			if(filepath) {
				/*
                if(!readfile(1, filepath)) {
					std::cout << "file attribute is invalid!" << std::endl;
					return false;
				}
                */
			} else {
				std::cout << "A model element must have a file attribute!" << std::endl;
				return false;
			}
			model = model->NextSiblingElement("model");
		}
		if(!loaded_model_at_least_once) {
			std::cout << "A models element must have at least one model child element!" << std::endl;
			return false;
		}
	} else {
		std::cout << "A group element must have models child element!" << std::endl;
		return false;
	}

    //loop through all subgroups

    tinyxml2::XMLElement *subgroup = root->FirstChildElement("group");
    for(subgroup; subgroup; subgroup = subgroup->NextSiblingElement("group")) {
        group sub(subgroup);
        sub_groups.push_back(sub);
        //super simple, right?
    }
    

    return true;
}


void group::print_group(const std::string prepend) {
    const char *transform_order_data = transform_order.data();
    char curated_order[4];
    size_t iter;
    for(iter = 0; iter < transform_order.size(); iter++) {
        curated_order[iter] = transform_order_data[iter];
    }
    curated_order[iter] = '\0';
    std::cout << prepend << "Transform order: " << curated_order << "\n";

    std::cout   << prepend << "Translate:\n"
                << prepend << "\tx: " << translate_x << "\n"
                << prepend << "\ty: " << translate_y << "\n"
                << prepend << "\tz: " << translate_z << "\n"
                << prepend << "Rotate:\n"
                << prepend << "\tangle: " << rotate_angle << "\n"
                << prepend << "\tx: " << rotate_x << "\n"
                << prepend << "\ty: " << rotate_y << "\n"
                << prepend << "\tz: " << rotate_z << "\n"
                << prepend << "Scale:\n"
                << prepend << "\tx: " << scale_x << "\n"
                << prepend << "\ty: " << scale_y << "\n"
                << prepend << "\tz: " << scale_z << "\n"
                << prepend << "Sub groups -> \n";
    group* sub_group_data = sub_groups.data();
    std::stringstream ss;
    ss << prepend << "|\t";
    std::string sub_prepend = ss.str();
    if(sub_groups.size() == 0) {
        std::cout << sub_prepend << "EMPTY!\n" << prepend << "<------------------------------------------------------>\n";
    }
    for(int j = 0; j < sub_groups.size(); j++) {
        sub_group_data[j].print_group(sub_prepend);
    }
    std::cout << std::flush;
}