#include "group.hpp"

group::group(tinyxml2::XMLElement *root) {
    transform_matrix = matrix4x4();

    if(!group::parse_group(root)) {
        throw FailedToParseGroupException(std::string("Failed to parse group element!"));
    }

    this->prepare_render();
}

void group::prepare_render() {
    if(is_ready_to_render) {
        std::cout << "Warning: group was already prepared for render." << std::endl;
        return;
    }

    size_t i;

    for(i = 0; i < mesh_count; i++) {
        std::vector<float> vertices = mesh_vertices_buffer.at(i);
        std::tuple<bool, std::vector<int>> indices = mesh_indices_buffer.at(i);

        std::tuple<bool, std::vector<float>> normals = mesh_normals_buffer.at(i);
        std::tuple<bool, std::vector<float>> texture_coordinates = mesh_tex_coords_buffer.at(i);

        //ignoring normals and tex coords for now

        if(!std::get<0>(indices)) {
            //normal vbo
            int vertex_count = vertices.size() / 3;
            const float *vertices_data = vertices.data();

            GLuint VBO;
            glGenBuffers(1, &VBO);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_data), vertices_data, GL_STATIC_DRAW);

            group_vbos.push_back(VBO);

            std::tuple<bool, GLuint> EBO(false, 0);
            group_ebos.push_back(EBO);

            vertex_or_index_count.push_back(vertex_count);
        }
        else {
            std::vector<int> indices_v = std::get<1>(indices);
            int index_count = indices_v.size();

            const float *vertices_data = vertices.data();
            const int *indices_data = indices_v.data();

            GLuint VBO, EBO;
            glGenBuffers(1, &VBO);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), vertices_data, GL_STATIC_DRAW);
            
            glGenBuffers(1, &EBO);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * indices_v.size(), indices_data, GL_STATIC_DRAW);

            std::tuple<bool, GLuint> EBO_tuple(true, EBO);

            group_vbos.push_back(VBO);
            group_ebos.push_back(EBO_tuple);

            vertex_or_index_count.push_back(index_count);
        }

        //if normals and tex coords exist, do something!
    }

    is_ready_to_render = true;
}

void group::render_group() {
    //rendering a group should render all subgroups
    glColor3f(color.x, color.y, color.z);

    glPushMatrix();

        glMultMatrixf(transform_matrix.get_data());

        for(unsigned int j = 0; j < mesh_count; j++) {
            GLuint VBO = group_vbos.at(j);
            std::tuple<bool, GLuint> EBO_t = group_ebos.at(j);
            int obj_count = vertex_or_index_count.at(j);

            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glVertexPointer(3, GL_FLOAT, 0, 0);

            if(!std::get<0>(EBO_t)) {
                glDrawArrays(GL_TRIANGLES, 0, obj_count);
            } 
            else {

                GLuint EBO = std::get<1>(EBO_t);

                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
                glDrawElements(GL_TRIANGLES, obj_count, GL_UNSIGNED_INT, NULL);
            }
        }

        for(size_t i = 0; i < sub_groups.size(); i++) {
            sub_groups.at(i).render_group();
        }

    glPopMatrix();

    //todo
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

                float t_x = 0, t_y = 0, t_z = 0;
                child->QueryFloatAttribute("x", &t_x);
                child->QueryFloatAttribute("y", &t_y);
                child->QueryFloatAttribute("z", &t_z);

                matrix4x4 T = matrix4x4::Translate(t_x, t_y, t_z);
                transform_matrix = transform_matrix * T;
            }
            else if(tag == "rotate") {

                float angle = 0, r_x = 0, r_y = 0, r_z = 0;
                child->QueryFloatAttribute("angle", &angle);
                child->QueryFloatAttribute("x", &r_x);
                child->QueryFloatAttribute("y", &r_y);
                child->QueryFloatAttribute("z", &r_z);
                
                matrix4x4 R = matrix4x4::Rotate(angle * (M_PI / 180.0f), r_x, r_y, r_z);

                transform_matrix = transform_matrix * R;
            } 
            else if(tag == "scale") {
                //transform_order.push_back('s');

                float s_x = 1, s_y = 1, s_z = 1;
                child->QueryFloatAttribute("x", &s_x);
                child->QueryFloatAttribute("y", &s_y);
                child->QueryFloatAttribute("z", &s_z);

                matrix4x4 S = matrix4x4::Scale(s_x, s_y, s_z);

                transform_matrix = transform_matrix * S;
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
				if(!parse_model_file(filepath)) {
                    std::cout << "Model file is invalid: " << filepath << std::endl;
                    return false;
                }
                mesh_count++;

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
	} 
    else {
		//std::cout << "A group element must have models child element!" << std::endl;
		//return false;
	}

    tinyxml2::XMLElement *color_element = root->FirstChildElement("color");
    if(color_element) {
        float r = 1.0f, g = 1.0f, b = 1.0f;

        color_element->QueryFloatAttribute("r", &r);
        color_element->QueryFloatAttribute("g", &g);
        color_element->QueryFloatAttribute("b", &b);

        color = vector3(r, g, b);
    } 
    else {
        //if no color element random generate the color.
        color = vector3(1.0f, 1.0f, 1.0f);
    }

    //loop through all subgroups

    tinyxml2::XMLElement *subgroup = root->FirstChildElement("group");
    for(subgroup; subgroup; subgroup = subgroup->NextSiblingElement("group")) {
        group sub(subgroup);
        sub_groups.push_back(sub);
    }
    

    return true;
}

bool group::parse_model_file(const char *filepath) {
    std::ifstream file(filepath);

    if(!file) {
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
    while (std::getline(file, line)) { // Read file line by line
        std::stringstream ss(line);  // Use stringstream to parse the line
        std::string token;

		if (line_index == 0) {
			const char* line_data = line.data();
            if(line_data[0] == '1') {
                i_flag = true;
                data_order.push_back('i');
            }
            if(line_data[1] == '1') {
                n_flag = true;
                data_order.push_back('n');
            }
            if(line_data[2] == '1') {
                t_flag = true;
                data_order.push_back('t');
            }
        }
        else if (line_index == 1) {  //vertices
            while (std::getline(ss, token, ';')) {
				float vertex_float = std::stof(token);
				vertices.push_back(vertex_float);
        	}

            mesh_vertices_buffer.push_back(vertices);
        }
        else {  //all the others
            if(data_order.size() == 0) break; //only indices
            if(data_order.at(line_index - 2) == 'i') {
                //read indices
                while (std::getline(ss, token, ';')) {
                    int index = std::stoi(token);
                    indices.push_back(index);
                }
                
                std::tuple<bool, std::vector<int>> index_t(true, indices);

                mesh_indices_buffer.push_back(index_t);
            }
            else if(data_order.at(line_index - 2) == 'n') {
                //read normals 
                //unused for now
            }
            else if(data_order.at(line_index - 2) == 't') {
                //read texture coordinates
                //unused for now
            }
        }
        
        line_index++;
    }

    if(!i_flag) {
        std::tuple<bool, std::vector<int>> index_t(false, indices);
        mesh_indices_buffer.push_back(index_t);
    }
    if(!n_flag) {
        std::tuple<bool, std::vector<float>> normal_t(false, normals);
        mesh_normals_buffer.push_back(normal_t);
    }
    if(!t_flag) {
        std::tuple<bool, std::vector<float>> coord_t(false, tex_coords);
        mesh_tex_coords_buffer.push_back(coord_t);
    }   

    file.close();

    return true;
}

std::vector<vector3> group::lock_positions(matrix4x4 parent_transform) {
    
    matrix4x4 current_full_transform = parent_transform * transform_matrix;
    
    vector3 resulting_origin = current_full_transform.apply_to_point(0, 0, 0);
    
    std::vector<vector3> lock_pos;
    lock_pos.push_back(resulting_origin);

    for(int i = 0; i < sub_groups.size(); i++) {
        std::vector<vector3> c_locks = sub_groups.at(i).lock_positions(current_full_transform);
        lock_pos.insert(lock_pos.end(), c_locks.begin(), c_locks.end());
    }

    return lock_pos;
}