#pragma once
#include "point_list_store.h"

struct tri_store
{
	// store the individual point
	int tri_id = 0;

	// Location
	glm::vec3 tript1_loc = glm::vec3(0);
	glm::vec3 tript2_loc = glm::vec3(0);
	glm::vec3 tript3_loc = glm::vec3(0);

	// color
	glm::vec3 tript1_color = glm::vec3(0);
	glm::vec3 tript2_color = glm::vec3(0);
	glm::vec3 tript3_color = glm::vec3(0);
};


class tri_list_store
{
public:
	geom_parameters* geom_param_ptr = nullptr;
	unsigned int tri_count = 0;
	std::vector<tri_store> triMap;

	tri_list_store();
	~tri_list_store();

	void init(geom_parameters* geom_param_ptr);
	void add_tri(int& tri_id, const glm::vec3& tript1_loc, const glm::vec3& tript2_loc, const glm::vec3& tript3_loc,
		glm::vec3& tript1_color, glm::vec3& tript2_color, glm::vec3& tript3_color);
	void set_buffer();
	void paint_triangles();
	void clear_triangles();
	void update_opengl_uniforms(bool set_modelmatrix, bool set_pantranslation, bool set_rotatetranslation,
		bool set_zoomtranslation, bool set_transparency, bool set_deflscale);
private:
	gBuffers tri_buffer;
	Shader tri_shader;

	void get_line_buffer(tri_store& tri, float* tri_vertices, unsigned int& tri_v_index, unsigned int* tri_vertex_indices, unsigned int& tri_i_index);


};