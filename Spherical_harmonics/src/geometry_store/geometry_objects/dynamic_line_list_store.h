#pragma once
#include "dynamic_point_list_store.h"


struct dynamic_line_store
{
	// store the individual point
	int line_id = -1;
		
	dynamic_point_store* start_pt = nullptr;
	dynamic_point_store* end_pt = nullptr;
	dynamic_line_store* next_line = nullptr;        // Next half-edge in the same face
	dynamic_line_store* twin_line = nullptr;        // Opposite half-edge
	dynamic_tri_store* face = nullptr;    // Face to the left of this half-edge

	glm::vec3 line_normal = glm::vec3(0); // Normal of the edge (It is based on the two faces its attached to)

};

class dynamic_line_list_store
{
public:
	geom_parameters* geom_param_ptr = nullptr;
	unsigned int dyn_line_count = 0;
	std::unordered_map<int, int> dyn_lineId_Map;
	std::vector<dynamic_line_store> dyn_lineMap;

	dynamic_line_list_store();
	~dynamic_line_list_store();
	void init(geom_parameters* geom_param_ptr);
	void add_line(const int& line_id, dynamic_point_store* start_pt,
		dynamic_point_store* end_pt, const glm::vec3& line_normal);
	dynamic_line_store* get_line(const int& dyn_line_id);

	void set_buffer();
	void paint_lines();
	void update_buffer(const int& dyn_index);
	void clear_lines();
	void update_opengl_uniforms(bool set_modelmatrix, bool set_pantranslation, bool set_rotatetranslation, 
		bool set_zoomtranslation, bool set_transparency, bool set_deflscale);
private:
	gBuffers dyn_line_buffer;
	Shader dyn_line_shader;

	void get_line_vertex_buffer(dynamic_line_store& ln, const int& dyn_index, 
		float* dyn_line_vertices, unsigned int& dyn_line_v_index);

	void get_line_index_buffer(unsigned int* dyn_line_vertex_indices, unsigned int& dyn_line_i_index);
};

