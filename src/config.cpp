#include "config.hpp"

config::config(const char *path) {
    if(!this->load(path)) {
        std::stringstream ss;
        ss << "Failed to load file at: " << path;
        throw FailedToLoadException(ss.str());
    }

	update_group_positions();

	cam = new camera(c_pos, c_lookat, c_up, query_group_postitions());
}

void config::print_info() {
    std::cout   << "Window settings:\n"
                << "\tWindow width: " << w_width << "\n"
                << "\tWindow height: " << w_height << "\n"
                << "\nCamera setttings:\n"
                << "\tCamera position:\n"
                << "\t\tx: " << c_pos.x << "\n"
                << "\t\ty: " << c_pos.y << "\n"
                << "\t\tz: " << c_pos.z << "\n"
                << "\tCamera lookAt:\n"
                << "\t\tx: " << c_lookat.x << "\n"
                << "\t\ty: " << c_lookat.y << "\n"
                << "\t\tz: " << c_lookat.z << "\n"
                << "\tCamera up vector:\n"
                << "\t\tx: " << c_up.x << "\n"
                << "\t\ty: " << c_up.y << "\n"
                << "\t\tz: " << c_up.z << "\n"
                << "\nProjection:\n"
                << "\tCamera fov: " << projection_attributes.x << "\n"
                << "\tCamera near plane: " << projection_attributes.y << "\n"
                << "\tCamera far plane: " << projection_attributes.z << "\n";

    std::cout << "\n" << std::flush; //same as endl
}

std::tuple<int, int> config::get_window_attributes() {
    return std::tuple<int, int>(w_width, w_height);
}

vector3 config::get_projection_settings() {
    return projection_attributes;
}

std::vector<group> config::get_root_groups() {
	return root_groups;
}

bool config::load(const char *filepath) {
    tinyxml2::XMLDocument doc;
	if(doc.LoadFile(filepath) != tinyxml2::XML_SUCCESS) {
		std::cout << "Failed to load XML config file at: " << filepath << std::endl;
		return false;
	}

	tinyxml2::XMLElement *root = doc.RootElement();
	if(root) {
		if(std::string(root->Value()) != "world") {
			std::cout << "Unexpected root element: " << root->Value() << std::endl;
			return false;
		}

		tinyxml2::XMLElement *window = root->FirstChildElement("window");
		if(window) {
			int width = -1, height = -1;
			window->QueryIntAttribute("width", &width);
			window->QueryIntAttribute("height", &height);

			if(width == -1 || height == -1) { //check if loaded
				std::cout << "There was a problem loading width\\height!" << std::endl;
				return false;
			}
			if(width <= 0 || height <= 0) { //check if valid
				std::cout << "Width\\Height is not valid!" << std::endl;
				return false;
			}

			w_width = width;
			w_height = height;
		} else {
			std::cout << "No window element!" << std::endl;
			return false;
		}

		tinyxml2::XMLElement *camera = root->FirstChildElement("camera");
		if(camera) {
			tinyxml2::XMLElement *position = camera->FirstChildElement("position");
			if(position) {
				float x = 0, y = 0, z = 0;
				position->QueryFloatAttribute("x", &x);
				position->QueryFloatAttribute("y", &y);
				position->QueryFloatAttribute("z", &z);

				c_pos = vector3(x, y, z);
			} else {
				std::cout << "No position element!" << std::endl;
				return false;
			}

			tinyxml2::XMLElement *lookat = camera->FirstChildElement("lookAt");
			if(lookat) {
				float x = 0, y = 0, z = 0;
				lookat->QueryFloatAttribute("x", &x);
				lookat->QueryFloatAttribute("y", &y);
				lookat->QueryFloatAttribute("z", &z);

				c_lookat = vector3(x, y, z);
			} else {
				std::cout << "No lookAt element!" << std::endl;
				return false;
			}

			tinyxml2::XMLElement *up = camera->FirstChildElement("up");
			if(up) {
				float x = 0, y = 0, z = 0;
				up->QueryFloatAttribute("x", &x);
				up->QueryFloatAttribute("y", &y);
				up->QueryFloatAttribute("z", &z);

				c_up = vector3(x, y, z);
			} else {
				std::cout << "No up element!" << std::endl;
				return false;
			}

			
			tinyxml2::XMLElement *projection = camera->FirstChildElement("projection");
			if(projection) {
				float fov = -1, near = -1, far = -1;
				projection->QueryFloatAttribute("fov", &fov);
				projection->QueryFloatAttribute("near", &near);
				projection->QueryFloatAttribute("far", &far);

				if(fov == -1 || near == -1 || far == -1) {
					std::cout << "Camera fov\\near plane\\far plane wasn't loaded!" << std::endl;
					return false;
				}
				if(fov <= 0 || near <= 0 || far <= 0) {
					std::cout << "Camera fov\\near plane\\far plane isn't valid!" << std::endl;
					return false;
				}

				projection_attributes = vector3(fov, near, far);
			} else {
				std::cout << "No projection element!" << std::endl;
				return false;
			}

		} else {
			std::cout << "No camera element!" << std::endl;
			return false;
		}

		bool loaded_group_at_least_once = false;
		tinyxml2::XMLElement *group_element = root->FirstChildElement("group");
		while(group_element) {
			loaded_group_at_least_once = true;

            group parsed(group_element);

            root_groups.push_back(parsed);
			
			group_element = group_element->NextSiblingElement("group");
		}

		if(!loaded_group_at_least_once) {
			std::cout << "At least one group element is mandatory!" << std::endl;
			return false;
		}

		return true;
	} else {
		std::cout << "Failed to load root element!" << std::endl;
		return false;
	}
}

void config::render_all_groups(frustum view_frustum, bool frustum_cull, bool render_bounding_spheres) {
	for(size_t i = 0; i < root_groups.size(); i++) {
		root_groups.at(i).render_group(view_frustum, frustum_cull, render_bounding_spheres);
	}
}

camera* config::get_config_camera_init() {
	return cam;
}

std::vector<vector3> config::query_group_postitions() {
	std::vector<vector3> l_pos;
	for(size_t i = 0; i < root_groups.size(); i++) {
		std::vector<vector3> c_locks = root_groups.at(i).query_group_positions();
		l_pos.insert(l_pos.end(), c_locks.begin(), c_locks.end());
	}
	return l_pos;
}

void config::update_group_positions() {
	for(size_t i = 0; i < root_groups.size(); i++) {
		root_groups.at(i).update_group_positions(matrix4x4::Identity());
	}
}