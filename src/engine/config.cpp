#include "engine/config.hpp"

config::config(const char *path)
{
	this->load(path);

	update_groups(0);

	cam = new camera(c_pos, c_lookat, c_up, query_group_postitions());
}

// getters

std::tuple<int, int> config::get_window_attributes()
{
	return std::tuple<int, int>(w_width, w_height);
}

vector3 config::get_projection_settings()
{
	return projection_attributes;
}

std::vector<group> config::get_root_groups()
{
	return root_groups;
}

camera *config::get_config_camera_init()
{
	return cam;
}

std::vector<vector3> config::query_group_postitions()
{
	std::vector<vector3> l_pos;
	for (size_t i = 0; i < root_groups.size(); i++)
	{
		std::vector<vector3> c_locks = root_groups.at(i).query_group_positions();
		l_pos.insert(l_pos.end(), c_locks.begin(), c_locks.end());
	}
	return l_pos;
}

// render / update groups

void config::render_all_groups(matrix4x4 &camera_transform, frustum &view_frustum, bool frustum_cull, bool render_bounding_spheres, bool draw_translation_path)
{
	for (size_t i = 0; i < root_groups.size(); i++)
	{
		root_groups.at(i).render_group(camera_transform, view_frustum, frustum_cull, render_bounding_spheres, draw_translation_path);
	}
}

void config::update_groups(int delta_time_ms)
{
	for (size_t i = 0; i < root_groups.size(); i++)
	{
		root_groups.at(i).update_group(delta_time_ms, matrix4x4::Identity());
	}
}

// private

void config::load(const char *filepath)
{
	std::stringstream ss; // string stream for errors and stuff
	tinyxml2::XMLDocument doc;
	if (doc.LoadFile(filepath) != tinyxml2::XML_SUCCESS)
	{
		ss << "Failed to load XML config file at: " << filepath;
		printer::print_exception(ss.str(), "config::load");
		throw FailedToLoadException(ss.str());
	}

	tinyxml2::XMLElement *root = doc.RootElement();
	if (root)
	{
		if (std::string(root->Value()) != "world")
		{
			ss << "Unexpected root element: " << root->Value();
			printer::print_exception(ss.str(), "config::load");
			throw FailedToLoadException(ss.str());
		}

		tinyxml2::XMLElement *window = root->FirstChildElement("window");
		if (window)
		{
			int width, height;

			if (window->QueryIntAttribute("width", &width) != tinyxml2::XML_SUCCESS || window->QueryIntAttribute("height", &height) != tinyxml2::XML_SUCCESS)
			{ // check if loaded
				ss << "There was a problem loading width\\height!";
				printer::print_exception(ss.str(), "config::load");
				throw FailedToLoadException(ss.str());
			}
			if (width <= 0 || height <= 0)
			{ // check if valid
				ss << "Width\\Height must be larger than 0!";
				printer::print_exception(ss.str(), "config::load");
				throw FailedToLoadException(ss.str());
			}

			w_width = width;
			w_height = height;
		}
		else
		{
			ss << "No window element!";
			printer::print_exception(ss.str(), "config::load");
			throw FailedToLoadException(ss.str());
		}

		tinyxml2::XMLElement *camera = root->FirstChildElement("camera");
		if (camera)
		{
			tinyxml2::XMLElement *position = camera->FirstChildElement("position");
			if (position)
			{
				float x, y, z;

				if (position->QueryFloatAttribute("x", &x) != tinyxml2::XML_SUCCESS || position->QueryFloatAttribute("y", &y) != tinyxml2::XML_SUCCESS || position->QueryFloatAttribute("z", &z) != tinyxml2::XML_SUCCESS)
				{
					ss << "x, y or z attribute of camera position is either missing or not a valid float!";
					printer::print_exception(ss.str(), "config::load");
					throw FailedToLoadException(ss.str());
				}

				c_pos = vector3(x, y, z);
			}
			else
			{
				ss << "No position element!";
				printer::print_exception(ss.str(), "config::load");
				throw FailedToLoadException(ss.str());
			}

			tinyxml2::XMLElement *lookat = camera->FirstChildElement("lookAt");
			if (lookat)
			{
				float x, y, z;

				if (lookat->QueryFloatAttribute("x", &x) != tinyxml2::XML_SUCCESS || lookat->QueryFloatAttribute("y", &y) != tinyxml2::XML_SUCCESS || lookat->QueryFloatAttribute("z", &z) != tinyxml2::XML_SUCCESS)
				{
					ss << "x, y or z attribute of camera lookat is either missing or not a valid float!";
					printer::print_exception(ss.str(), "config::load");
					throw FailedToLoadException(ss.str());
				}

				c_lookat = vector3(x, y, z);
			}
			else
			{
				ss << "No lookAt element!";
				printer::print_exception(ss.str(), "config::load");
				throw FailedToLoadException(ss.str());
			}

			tinyxml2::XMLElement *up = camera->FirstChildElement("up");
			if (up)
			{
				float x, y, z;

				if (up->QueryFloatAttribute("x", &x) != tinyxml2::XML_SUCCESS || up->QueryFloatAttribute("y", &y) != tinyxml2::XML_SUCCESS || up->QueryFloatAttribute("z", &z) != tinyxml2::XML_SUCCESS)
				{
					ss << "x, y or z attribute of camera up is either missing or not a valid float!";
					printer::print_exception(ss.str(), "config::load");
					throw FailedToLoadException(ss.str());
				}

				c_up = vector3(x, y, z);
			}
			else
			{
				ss << "No up element!";
				printer::print_exception(ss.str(), "config::load");
				throw FailedToLoadException(ss.str());
			}

			tinyxml2::XMLElement *projection = camera->FirstChildElement("projection");
			if (projection)
			{
				float fov = -1, near_p = -1, far_p = -1;
				projection->QueryFloatAttribute("fov", &fov);
				projection->QueryFloatAttribute("near", &near_p);
				projection->QueryFloatAttribute("far", &far_p);

				if (projection->QueryFloatAttribute("fov", &fov) != tinyxml2::XML_SUCCESS || projection->QueryFloatAttribute("near", &near_p) != tinyxml2::XML_SUCCESS || projection->QueryFloatAttribute("far", &far_p) != tinyxml2::XML_SUCCESS)
				{
					ss << "Camera fov\\near plane\\far plane attribute of camera projection was either missing or not a valid float!";
					printer::print_exception(ss.str(), "config::load");
					throw FailedToLoadException(ss.str());
				}
				if (fov <= 0 || near_p <= 0 || far_p <= 0)
				{
					ss << "Camera fov\\near plane\\far plane must be larger than 0!";
					printer::print_exception(ss.str(), "config::load");
					throw FailedToLoadException(ss.str());
				}

				projection_attributes = vector3(fov, near_p, far_p);
			}
			else
			{
				ss << "No projection element!";
				printer::print_exception(ss.str(), "config::load");
				throw FailedToLoadException(ss.str());
			}
		}
		else
		{
			ss << "No camera element!";
			printer::print_exception(ss.str(), "config::load");
			throw FailedToLoadException(ss.str());
		}

		bool loaded_group_at_least_once = false;
		tinyxml2::XMLElement *group_element = root->FirstChildElement("group");
		while (group_element)
		{
			loaded_group_at_least_once = true;

			group parsed(group_element);

			root_groups.push_back(parsed);

			group_element = group_element->NextSiblingElement("group");
		}
		if (!loaded_group_at_least_once)
		{
			ss << "At least one group element is mandatory!";
			printer::print_exception(ss.str(), "config::load");
			throw FailedToLoadException(ss.str());
		}

		tinyxml2::XMLElement *lights = root->FirstChildElement("lights");
		if (lights)
		{
			static GLenum light_enums[] = {GL_LIGHT0, GL_LIGHT1, GL_LIGHT2, GL_LIGHT3, GL_LIGHT4, GL_LIGHT5, GL_LIGHT6, GL_LIGHT7};
			size_t current_light = 0;

			bool has_loaded_light = false;
			tinyxml2::XMLElement *light_elem = lights->FirstChildElement("light");
			while (light_elem)
			{
				if (current_light > 7)
				{
					printer::print_exception("More than 8 lights were found! Number of lights MUST be less than or equal to 8.", "group::load");
					throw FailedToLoadException("");
				}

				has_loaded_light = true;

				const char *type;
				tinyxml2::XMLError type_result = light_elem->QueryStringAttribute("type", &type);

				if (type_result != tinyxml2::XML_SUCCESS)
				{
					printer::print_exception("light element either doesn't have a type attribute or it is not a valid string!", "group::load");
					throw FailedToLoadException("");
				}

				vector3 pos, dir;
				unsigned char type_c;
				float cutoff = 45;

				if (std::string("point").compare(type) == 0)
				{
					type_c = 'p';
					float posX, posY, posZ;
					if (light_elem->QueryFloatAttribute("posX", &posX) != tinyxml2::XML_SUCCESS || light_elem->QueryFloatAttribute("posY", &posY) != tinyxml2::XML_SUCCESS || light_elem->QueryFloatAttribute("posZ", &posZ) != tinyxml2::XML_SUCCESS)
					{
						printer::print_exception("posX, posY or posZ attribute of point light element is either missing or not a valid float!", "group::load");
						throw FailedToLoadException("");
					}
					pos = vector3(posX, posY, posZ);
				}
				else if (std::string("directional").compare(type) == 0)
				{
					type_c = 'd';
					float dirX, dirY, dirZ;
					if (light_elem->QueryFloatAttribute("dirX", &dirX) != tinyxml2::XML_SUCCESS || light_elem->QueryFloatAttribute("dirY", &dirY) != tinyxml2::XML_SUCCESS || light_elem->QueryFloatAttribute("dirZ", &dirZ) != tinyxml2::XML_SUCCESS)
					{
						printer::print_exception("dirX, dirY or dirZ attribute of directional light element is either missing or not a valid float!", "group::load");
						throw FailedToLoadException("");
					}
					dir = vector3(dirX, dirY, dirZ);
				}
				else if (std::string("spotlight").compare(type) == 0)
				{
					type_c = 's';
					float posX, posY, posZ;
					float dirX, dirY, dirZ;
					float cutoff_attr;

					if (light_elem->QueryFloatAttribute("posX", &posX) != tinyxml2::XML_SUCCESS || light_elem->QueryFloatAttribute("posY", &posY) != tinyxml2::XML_SUCCESS || light_elem->QueryFloatAttribute("posZ", &posZ) != tinyxml2::XML_SUCCESS)
					{
						printer::print_exception("posX, posY or posZ attribute of spotlight light element is either missing or not a valid float!", "group::load");
						throw FailedToLoadException("");
					}

					if (light_elem->QueryFloatAttribute("dirX", &dirX) != tinyxml2::XML_SUCCESS || light_elem->QueryFloatAttribute("dirY", &dirY) != tinyxml2::XML_SUCCESS || light_elem->QueryFloatAttribute("dirZ", &dirZ) != tinyxml2::XML_SUCCESS)
					{
						printer::print_exception("dirX, dirY or dirZ attribute of spotlight light element is either missing or not a valid float!", "group::load");
						throw FailedToLoadException("");
					}

					if (light_elem->QueryFloatAttribute("cutoff", &cutoff_attr) != tinyxml2::XML_SUCCESS)
					{
						printer::print_exception("cutoff attribute of spolight light element is either missing or not a valid float!", "group::load");
						throw FailedToLoadException("");
					}

					pos = vector3(posX, posY, posZ);
					dir = vector3(dirX, dirY, dirZ);
					cutoff = cutoff_attr;
				}
				else
				{
					printer::print_exception("type attribute of light element is not valid!");
					throw FailedToLoadException("");
				}

				light l = light(light_enums[current_light], type_c, pos, dir, cutoff);
				this->lights.push_back(l);

				current_light++;
				light_elem = light_elem->NextSiblingElement("light");
			}
			if (!has_loaded_light)
			{
				printer::print_warning("USE_LIGHTING is defined but no light elements were found. Defaulting to single point light at origin.");
			}
		}
		else
		{
			printer::print_warning("USE_LIGHTING is defined but no lights element was found. Defaulting to single point light at origin.");
			light l = light(GL_LIGHT0, 'p', vector3(), vector3(), 0);
			this->lights.push_back(l);
		}
	}
	else
	{
		ss << "Failed to load root element!";
		printer::print_exception(ss.str(), "config::load");
		throw FailedToLoadException(ss.str());
	}
}

void config::apply_lights()
{
	for (size_t i = 0; i < lights.size(); i++)
	{
		lights.at(i).apply_light();
	}
}

// debug

void config::print_info()
{
	std::cout << "Window settings:\n"
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

	std::cout << "\n"
			  << std::flush; // same as endl
}
