#include "transforms/translation_dynamic.hpp"

translation_dynamic::translation_dynamic(float total_time, bool align, std::vector<vector3> points, int path_divisions)
{
	this->total_time = total_time;
	this->align = align;
	this->points = points;

	this->animation_timer = 0;

	float time_delta = 1.0f / (float)path_divisions;
	for (int i = 0; i < path_divisions; i++)
	{
		float time_alpha = i * time_delta;

		std::tuple<vector3, vector3> pos_deriv = p_d_on_curve(time_alpha);
		this->path_points.push_back(std::get<0>(pos_deriv));
		this->path_derivs.push_back(std::get<1>(pos_deriv));
	}
}

void translation_dynamic::update(int delta_time_ms)
{
	vector3 x;
	vector3 y = vector3(0.0f, 1.0f, 0.0f);
	vector3 z;

	this->animation_timer += delta_time_ms;

	float time_alpha = ((float)animation_timer / 1000.0f) / total_time;

	if (time_alpha >= 1)
	{
		animation_timer -= (int)total_time * 1000; // round down!
		time_alpha = ((float)animation_timer / 1000.0f) / total_time;
	}

	std::tuple<vector3, vector3> pos_deriv = p_d_on_curve(time_alpha);

	vector3 current_position = std::get<0>(pos_deriv);
	vector3 current_derivative = std::get<1>(pos_deriv);

	if (!align)
		this->translation_matrix = matrix4x4::Translate(current_position);
	else
	{
		x = current_derivative;
		z = vector3::cross(x, y);
		y = vector3::cross(z, x);

		x.normalize();
		y.normalize();
		z.normalize();

		matrix4x4 t_component = matrix4x4::Translate(current_position);
		matrix4x4 r_component = matrix4x4::Rotate(x, y, z);

		this->translation_matrix = t_component * r_component;
	}
}

matrix4x4 translation_dynamic::get_translation()
{
	return this->translation_matrix;
}

std::tuple<vector3, vector3> translation_dynamic::p_d_on_curve(float time_alpha)
{
	float t = time_alpha * this->points.size();
	int index = floor(t);
	t = t - index;

	int POINT_COUNT = this->points.size();

	int indices[4];
	indices[0] = (index + POINT_COUNT - 1) % POINT_COUNT;
	indices[1] = (indices[0] + 1) % POINT_COUNT;
	indices[2] = (indices[1] + 1) % POINT_COUNT;
	indices[3] = (indices[2] + 1) % POINT_COUNT;

	return p_d_on_segment(t, points.at(indices[0]), points.at(indices[1]), points.at(indices[2]), points.at(indices[3]));
}

std::tuple<vector3, vector3> translation_dynamic::p_d_on_segment(float segment_time_alpha, vector3 p0, vector3 p1, vector3 p2, vector3 p3)
{
	matrix4x4 m = matrix4x4::Catmul_rom();

	vector4 a_x; // = m * px
	vector4 a_y; // = m * py
	vector4 a_z; // = m * pz

	vector4 p_x = vector4(p0.x, p1.x, p2.x, p3.x);
	vector4 p_y = vector4(p0.y, p1.y, p2.y, p3.y);
	vector4 p_z = vector4(p0.z, p1.z, p2.z, p3.z);

	a_x = m * p_x;
	a_y = m * p_y;
	a_z = m * p_z;

	vector4 t_v = vector4(segment_time_alpha * segment_time_alpha * segment_time_alpha, segment_time_alpha * segment_time_alpha, segment_time_alpha, 1);
	vector4 td_v = vector4(segment_time_alpha * segment_time_alpha * 3, segment_time_alpha * 2, 1, 0);

	vector3 pos(t_v * a_x, t_v * a_y, t_v * a_z);
	vector3 deriv(td_v * a_x, td_v * a_y, td_v * a_z);

	return std::tuple<vector3, vector3>(pos, deriv);
}

void translation_dynamic::draw_path()
{
	glBegin(GL_LINE_LOOP);
	glColor3f(0.6f, 0.6f, 0.6f);
	for (size_t i = 0; i < this->path_points.size(); i++)
	{
		vector3 p = this->path_points.at(i);
		glVertex3f(p.x, p.y, p.z);
	}
	glEnd();

	glBegin(GL_LINES);
	for (size_t i = 0; i < this->path_points.size(); i++)
	{
		vector3 p = this->path_points.at(i);
		vector3 d = this->path_derivs.at(i);

		glVertex3f(p.x, p.y, p.z);

		glColor3f(0.9f, 0.3f, 0.3f);
		glVertex3f(p.x + d.x, p.y + d.y, p.z + d.z);
	}
	glEnd();
}

translation_dynamic::operator const matrix4x4() const
{
	return this->translation_matrix;
}