#include "config.hpp"

config::config(const char *path) {
    if(!this->load(path)) {
        std::stringstream ss;
        ss << "Failed to load file at: " << path;
        throw FailedToLoadException(ss.str());
    }
}

void config::print_info() {
    std::cout   << "Window settings:\n"
                << "\tWindow width: " << w_width << "\n"
                << "\tWindow height: " << w_height << "\n"
                << "\nCamera setttings:\n"
                << "\tCamera position:\n"
                << "\t\tx: " << c_pos_x << "\n"
                << "\t\ty: " << c_pos_y << "\n"
                << "\t\tz: " << c_pos_z << "\n"
                << "\tCamera lookAt:\n"
                << "\t\tx: " << c_lookat_x << "\n"
                << "\t\ty: " << c_lookat_y << "\n"
                << "\t\tz: " << c_lookat_z << "\n"
                << "\tCamera up vector:\n"
                << "\t\tx: " << c_up_x << "\n"
                << "\t\ty: " << c_up_y << "\n"
                << "\t\tz: " << c_up_z << "\n"
                << "\nProjection:\n"
                << "\tCamera fov: " << c_fov << "\n"
                << "\tCamera near plane: " << c_near_plane << "\n"
                << "\tCamera far plane: " << c_far_plane << "\n";

    std::cout << "\n" << std::flush; //same as endl
}

std::tuple<int, int> config::get_window_attributes() {
    return std::tuple<int, int>(w_width, w_height);
}

std::tuple<float, float, float> config::get_camera_position() {
    return std::tuple<float, float, float>(c_pos_x, c_pos_y, c_pos_z);
}

std::tuple<float, float, float> config::get_camera_lookAt() {
    return std::tuple<float, float, float>(c_lookat_x, c_lookat_y, c_lookat_z);
}

std::tuple<float, float, float> config::get_camera_up_vector() {
    return std::tuple<float, float, float>(c_up_x, c_up_y, c_up_z);
}

std::tuple<float, float, float> config::get_projection_settings() {
    return std::tuple<float, float, float>(c_fov, c_near_plane, c_far_plane);
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
			std::cout << "Unexpected root element." << std::endl;
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

			std::cout << "Width: " << width << std::endl;
			std::cout << "Height: " << height << std::endl;
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

				c_pos_x = x;
				c_pos_y = y;
				c_pos_z = z;
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

				c_lookat_x = x;
				c_lookat_y = y;
				c_lookat_z = z;
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

				c_up_x = x;
				c_up_y = y;
				c_up_z = z;
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

				c_fov = fov;
				c_near_plane = near;
				c_far_plane = far;
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

void config::prepare_all_groups() {
	for(size_t i = 0; i < root_groups.size(); i++) {
		root_groups.at(i).prepare_render();
	}
}

void config::render_all_groups() {
	for(size_t i = 0; i < root_groups.size(); i++) {
		root_groups.at(i).render_group();
	}
}