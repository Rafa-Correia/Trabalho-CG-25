#include "transforms/translation_dynamic.hpp"

translation_dynamic::translation_dynamic(float total_time, bool align, std::vector<vector3> points, int path_divisions) {
    this->total_time = total_time;
    this->align = align;
    this->points = points;

    this->animation_timer = 0;

	float time_delta = 1.0f / (float)path_divisions;
    for(int i = 0; i < path_divisions; i++) {
        float time_alpha = i*time_delta;

        path_points.push_back(position_on_curve(time_alpha));
    }
}

void translation_dynamic::update(int delta_time_ms) {
    this->animation_timer += delta_time_ms;

    float time_alpha = ((float)animation_timer / 1000.0f) / total_time;

    if(time_alpha >= 1) {
        animation_timer -= (int)total_time * 1000;  //round down!
        time_alpha = ((float)animation_timer / 1000.0f) / total_time;
    }

    vector3 current_position = position_on_curve(time_alpha);

    this->translation_matrix = matrix4x4::Translate(current_position);
}

matrix4x4 translation_dynamic::get_translation() {
    return this->translation_matrix;
}

vector3 translation_dynamic::position_on_curve(float time_alpha) {
    float t = time_alpha * this->points.size();
    int index = floor(t);
    t = t - index;

    int POINT_COUNT = this->points.size();

    int indices[4]; 
	indices[0] = (index + POINT_COUNT-1)%POINT_COUNT;	
	indices[1] = (indices[0]+1)%POINT_COUNT;
	indices[2] = (indices[1]+1)%POINT_COUNT; 
	indices[3] = (indices[2]+1)%POINT_COUNT;

    return position_on_segment(t, points.at(indices[0]), points.at(indices[1]), points.at(indices[2]), points.at(indices[3]));
}

void translation_dynamic::multMatrixVector(float *m, float *v, float *res) {

	for (int j = 0; j < 4; ++j) {
		res[j] = 0;
		for (int k = 0; k < 4; ++k) {
			res[j] += v[k] * m[j * 4 + k];
		}
	}
}

float * translation_dynamic::dataFromMatrix(float m[4][4]) {
	float data[16];
	for(int i = 0; i < 4; i++) {
		data[i*4] = m[i][0];
		data[i*4 + 1] = m[i][1];
		data[i*4 + 2] = m[i][2];
		data[i*4 + 3] = m[i][3];
	}

	return data;
}

vector3 translation_dynamic::position_on_segment(float segment_time_alpha, vector3 p0, vector3 p1, vector3 p2, vector3 p3) {
    float m[4][4] = {	{-0.5f,  1.5f, -1.5f,  0.5f},
						{ 1.0f, -2.5f,  2.0f, -0.5f},
						{-0.5f,  0.0f,  0.5f,  0.0f},
						{ 0.0f,  1.0f,  0.0f,  0.0f}};
			
	// Compute A = M * P
	float a_x[4]; // = m * px
	float a_y[4]; // = m * py
	float a_z[4]; // = m * pz

	float p_x[4] = {p0.x, p1.x, p2.x, p3.x};
	float p_y[4] = {p0.y, p1.y, p2.y, p3.y};
	float p_z[4] = {p0.z, p1.z, p2.z, p3.z}; 
    /* 
    for (int j = 0; j < 4; ++j) {
		res[j] = 0;
		for (int k = 0; k < 4; ++k) {
			res[j] += v[k] * m[j * 4 + k];
		}
	}
    */

    multMatrixVector(dataFromMatrix(m), p_x, a_x);
	multMatrixVector(dataFromMatrix(m), p_y, a_y);
	multMatrixVector(dataFromMatrix(m), p_z, a_z);

	//normalize(a_x);
	//normalize(a_y);
	//normalize(a_z);
	
	// Compute pos = T * A

	float t_v[4] = {segment_time_alpha * segment_time_alpha * segment_time_alpha, segment_time_alpha * segment_time_alpha, segment_time_alpha, 1};

    float pos[3] = {0};

	for(int i = 0; i < 4; i++) {
		pos[0] += t_v[i] * a_x[i];
		pos[1] += t_v[i] * a_y[i];
		pos[2] += t_v[i] * a_z[i];
	}

    return vector3(pos[0], pos[1], pos[2]);
}

void translation_dynamic::draw_path() {
    glBegin(GL_LINE_LOOP);
        glColor3f(0.6f, 0.6f, 0.6f);
        for(int i = 0; i < path_points.size(); i++) {
            vector3 p = path_points.at(i);
            glVertex3f(p.x, p.y, p.z);
        }
    glEnd();
}