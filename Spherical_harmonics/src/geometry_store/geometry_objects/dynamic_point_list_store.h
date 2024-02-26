#pragma once
#include "../geometry_buffers/gBuffers.h"
#include "../geom_parameters.h"

struct dynamic_tri_store; // Forward declaration

struct dynamic_point_store
{
	// store the individual point
	int point_id = -1;
	glm::vec3 point_loc = glm::vec3(0);
	glm::vec3 point_normal = glm::vec3(0); 
	std::vector<glm::vec3> point_offset; // Dynamic point offset
	std::vector<double> point_offset_mag; // Dynamic point offset values

};

class dynamic_point_list_store
{
public:
	geom_parameters* geom_param_ptr = nullptr;
	unsigned int dyn_point_count = 0;
	std::unordered_map<int, int> dyn_pointId_Map;
	std::vector<dynamic_point_store> dyn_pointMap;

	dynamic_point_list_store();
	~dynamic_point_list_store();

	void init(geom_parameters* geom_param_ptr);
	void add_point(const int& point_id,const glm::vec3& point_loc,const std::vector<glm::vec3>& point_offset,
		const std::vector<double>& point_offset_mag);
	dynamic_point_store* get_point(const int& point_id);

	void set_buffer();
	void paint_points();
	void update_buffer(const int& dyn_index);
	void clear_points();
	void update_opengl_uniforms(bool set_modelmatrix, bool set_pantranslation, bool set_rotatetranslation,
		bool set_zoomtranslation, bool set_transparency, bool set_deflscale);
private:
	gBuffers dyn_point_buffer;
	Shader dyn_point_shader;

	void get_point_vertex_buffer(dynamic_point_store& pt, const int& dyn_index, 
		float* dyn_point_vertices, unsigned int& dyn_point_v_index);

	void get_point_index_buffer(unsigned int* dyn_point_indices, unsigned int& dyn_point_i_index);
};
