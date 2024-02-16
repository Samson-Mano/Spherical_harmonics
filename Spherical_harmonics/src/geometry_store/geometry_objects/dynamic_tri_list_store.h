#pragma once
#include "../geometry_buffers/gBuffers.h"
#include "../geom_parameters.h"


struct dynamic_tri_store
{
	// store the individual point
	int tri_id = 0;

	// Location
	glm::vec3 tript1_loc = glm::vec3(0);
	glm::vec3 tript2_loc = glm::vec3(0);
	glm::vec3 tript3_loc = glm::vec3(0);

	// offset
	std::vector<glm::vec3> tript1_offset;
	std::vector<glm::vec3> tript2_offset;
	std::vector<glm::vec3> tript3_offset;

	// Deflection value
	std::vector<double> tript1_offset_val;
	std::vector<double> tript2_offset_val;
	std::vector<double> tript3_offset_val;

};


class dynamic_tri_list_store
{
public:
	geom_parameters* geom_param_ptr = nullptr;
	unsigned int dyn_tri_count = 0;
	std::vector<dynamic_tri_store> dyn_triMap;

	dynamic_tri_list_store();
	~dynamic_tri_list_store();
	void init(geom_parameters* geom_param_ptr);
	void add_tri(int& tri_id, const glm::vec3& tript1_loc, const glm::vec3& tript2_loc, const glm::vec3& tript3_loc,
		const std::vector<glm::vec3>& tript1_offset, const std::vector<glm::vec3>& tript2_offset, const std::vector<glm::vec3>& tript3_offset,
		const std::vector<double>& tript1_offset_mag, const std::vector<double>& tript2_offset_mag, const std::vector<double>& tript3_offset_mag);

	void set_buffer();
	void paint_triangles();
	void paint_triangles(const int& dyn_index);
	void update_buffer(const int& dyn_index);
	void clear_triangles();
	void update_opengl_uniforms(bool set_modelmatrix, bool set_pantranslation, bool set_rotatetranslation,
		bool set_zoomtranslation, bool set_transparency, bool set_deflscale);
private:
	gBuffers dyn_tri_buffer;
	Shader dyn_tri_shader;

	void get_tri_vertex_buffer(dynamic_tri_store& tri, const int& dyn_index,
		float* dyn_tri_vertices, unsigned int& dyn_tri_v_index);

	void get_tri_index_buffer(unsigned int* dyn_tri_vertex_indices, unsigned int& dyn_tri_i_index);
};