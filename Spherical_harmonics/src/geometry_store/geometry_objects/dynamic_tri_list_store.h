#pragma once
#include "dynamic_line_list_store.h"

struct dynamic_tri_store
{
	// store the individual point
	int tri_id = -1;

	// Dynamic Edges
	dynamic_line_store* edge1 = nullptr; // Triangle edge 1
	dynamic_line_store* edge2 = nullptr; // Triangle edge 2
	dynamic_line_store* edge3 = nullptr; // Triangle edge 3

	glm::vec3 face_normal = glm::vec3(0); // Normal of face
};


class dynamic_tri_list_store
{
public:
	geom_parameters* geom_param_ptr = nullptr;
	unsigned int dyn_tri_count = 0;
	std::unordered_map<int, int> dyn_triId_Map;
	std::vector<dynamic_tri_store*> dyn_triMap;

	dynamic_tri_list_store();
	~dynamic_tri_list_store();
	void init(geom_parameters* geom_param_ptr);
	void add_tri(const int& tri_id, dynamic_line_store* edge1, dynamic_line_store* edge2, 
		dynamic_line_store* edge3);
	dynamic_tri_store* get_triangle(const int& dyn_tri_id);

	void set_buffer();
	void paint_triangles();
	void update_buffer(const int& dyn_index);
	void clear_triangles();
	void update_opengl_uniforms(bool set_modelmatrix, bool set_viewmatrix, bool set_deflscale);
private:
	gBuffers dyn_tri_buffer;
	Shader dyn_tri_shader;

	void get_tri_vertex_buffer(dynamic_tri_store* tri, const int& dyn_index,
		float* dyn_tri_vertices, unsigned int& dyn_tri_v_index);

	void get_tri_index_buffer(unsigned int* dyn_tri_vertex_indices, unsigned int& dyn_tri_i_index);
};