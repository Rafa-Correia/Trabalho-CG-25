#include "group.hpp"

group::group(tinyxml2::XMLElement *root) {
    if(!group::parse_group(root)) {
        throw FailedToParseGroupException(std::string("Failed to parse group element!"));
    }

    color_r = (float)rand() / (float)RAND_MAX;
    color_g = (float)rand() / (float)RAND_MAX;
    color_b = (float)rand() / (float)RAND_MAX;
}

void group::prepare_render() {
    //prepare render of root group prepares render of all sub groups
    if(ready_to_render) {
        std::cout << "Warning: Group is already prepared!" << std::endl;
        return;
    }

    std::cout << "Preparing group..." << std::endl;

    size_t i;

    for(i = 0; i < mesh_count; i++) {
        std::cout << "Mesh " << i << std::endl;
        std::vector<float> vertices = mesh_vertices_buffer.at(i);
        std::tuple<bool, std::vector<int>> indices = mesh_indices_buffer.at(i);

        std::tuple<bool, std::vector<float>> normals = mesh_normals_buffer.at(i);
        std::tuple<bool, std::vector<float>> texture_coordinates = mesh_tex_coords_buffer.at(i);

        //ignoring normals and tex coords for now

        if(!std::get<0>(indices)) {
            std::cout << "VBO only!" << std::endl;
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
            //index as well
            std::cout << "VBO and EBO!" << std::endl;
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

    std::cout << "Finished preparing all meshes!" << std::endl;

    ready_to_render = true;

    //prepare render of all subgroups
    for(i = 0; i < sub_groups.size(); i++) {
        sub_groups.at(i).prepare_render();
    }
}

void group::render_group() {
    if(!ready_to_render) {
        throw new NotReadyToRenderException("prepare_render() must be called first!");
    }

    //std::cout << "Rendering group..." << std::endl;

    //rendering a group should render all subgroups
    glColor3f(color_r, color_g, color_b);

    glPushMatrix();

        for(size_t i = 0; i < transform_order.size(); i++) {
            if(transform_order.at(i) == 't') {
                glTranslatef(translate_x, translate_y, translate_z);
            }
            else if(transform_order.at(i) == 'r') {
                glRotatef(rotate_angle, rotate_x, rotate_y, rotate_z);
            }
            else if(transform_order.at(i) == 's') {
                glScalef(scale_x, scale_y, scale_z);
            }
        }

        for(unsigned int j = 0; j < mesh_count; j++) {
            //std::cout << "Rendering mesh " << j << "..." << std::endl;
            GLuint VBO = group_vbos.at(j);
            std::tuple<bool, GLuint> EBO_t = group_ebos.at(j);
            int obj_count = vertex_or_index_count.at(j);

            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glVertexPointer(3, GL_FLOAT, 0, 0);

            if(!std::get<0>(EBO_t)) {
                //only vbo
                glDrawArrays(GL_TRIANGLES, 0, obj_count);
                //std::cout << "Rendered mesh " << j << " (VBO)" << std::endl;
            } 
            else {
                //with indices
                //std::cout << "obj_count -> " << obj_count << std::endl;

                GLuint EBO = std::get<1>(EBO_t);

                //std::cout << "EBO -> " << EBO << std::endl;

                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
                glDrawElements(GL_TRIANGLES, obj_count, GL_UNSIGNED_INT, NULL);

                //std::cout << "Rendered mesh " << j << " (VBO, EBO)" << std::endl;
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
				parse_model_file(filepath);
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

bool group::parse_model_file(const char *filepath) {
    std::cout << "Trying to load file \"" << filepath << "\"" << std::endl;

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

        std::cout << line_index << std::endl;
        // Split the line by ';'
		if (line_index == 0) {
			const char* line_data = line.data();
            if(line_data[0] == '1') {
                i_flag = true;
                data_order.push_back('i');
                std::cout << "Indices -> True" << std::endl;
            }
            if(line_data[1] == '1') {
                n_flag = true;
                data_order.push_back('n');
                std::cout << "Normals -> True" << std::endl;
            }
            if(line_data[2] == '1') {
                t_flag = true;
                data_order.push_back('t');
                std::cout << "Tex -> True" << std::endl;
            }
        }
        else if (line_index == 1) {  //vertices
            std::cout << "Reading vertices..." << std::endl;
            //read vertices
            while (std::getline(ss, token, ';')) {
            	//std::cout << "i: " << line_index << " | " << "Parsed token: " << token << '\n'; // Output each token
				float vertex_float = std::stof(token);
				vertices.push_back(vertex_float);
        	}

            mesh_vertices_buffer.push_back(vertices);
        }
        else {  //all the others
            if(data_order.size() == 0) break; //only indices
            if(data_order.at(line_index - 2) == 'i') {
                std::cout << "Reading indices..." << std::endl;
                //read indices
                while (std::getline(ss, token, ';')) {
                    //std::cout << "i: " << line_index << " | " << "Parsed token: " << token << '\n'; // Output each token
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
        std::cout << "Indices -> False" << std::endl;
    }
    if(!n_flag) {
        std::tuple<bool, std::vector<float>> normal_t(false, normals);
        mesh_normals_buffer.push_back(normal_t);
        std::cout << "Normals -> False" << std::endl;
    }
    if(!t_flag) {
        std::tuple<bool, std::vector<float>> coord_t(false, tex_coords);
        mesh_tex_coords_buffer.push_back(coord_t);
        std::cout << "Tex -> False" << std::endl;
    }   

    file.close();

    std::cout << "Done!" << std::endl;

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
    std::cout << prepend << "\n" << prepend << "Transform order: " << curated_order << "\n" << prepend << "\n";

    for(size_t i = 0; i < transform_order.size(); i++) {
        if(curated_order[i] == 't') {std::cout << "Indices -> False" << std::endl;
            std::cout   << prepend << "Translate:\n"
                        << prepend << "\tx: " << translate_x << "\n"
                        << prepend << "\ty: " << translate_y << "\n"
                        << prepend << "\tz: " << translate_z << "\n" << prepend << "\n";
        }
        else if(curated_order[i] == 'r') {
            std::cout   << prepend << "Rotate:\n"
                        << prepend << "\tangle: " << rotate_angle << "\n"
                        << prepend << "\tx: " << rotate_x << "\n"
                        << prepend << "\ty: " << rotate_y << "\n"
                        << prepend << "\tz: " << rotate_z << "\n" << prepend << "\n"; 
        }
        else if(curated_order[i] == 's') {
            std::cout   << prepend << "Scale:\n"
                        << prepend << "\tx: " << scale_x << "\n"
                        << prepend << "\ty: " << scale_y << "\n"
                        << prepend << "\tz: " << scale_z << "\n" << prepend << "\n";
        }
    }
    
    if(sub_groups.size() != 0) {
        std::stringstream ss;
        ss << prepend << "|\t";
        std::string sub_prepend = ss.str();
        std::cout << prepend << "Sub groups -> \n";

        for(int j = 0; j < sub_groups.size(); j++) {
            sub_groups.at(j).print_group(sub_prepend);
        }
    }
    std::cout << prepend << "<------------------------------------------------------>\n";
    
    std::cout << std::flush;
}