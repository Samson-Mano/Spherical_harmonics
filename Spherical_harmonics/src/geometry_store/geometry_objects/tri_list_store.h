#pragma once
#include "point_list_store.h"
#include "line_list_store.h"

struct tri_store
{
	// store the individual point
	int tri_id = -1;

	// Edges
	line_store* edge1 = nullptr; // Triangle edge 1
	line_store* edge2 = nullptr; // Triangle edge 2
	line_store* edge3 = nullptr; // Triangle edge 3
};


class tri_list_store
{
public:
	geom_parameters* geom_param_ptr = nullptr;
	unsigned int tri_count = 0;
	std::unordered_map<int, int> triId_Map;
	std::vector<tri_store> triMap;

	tri_list_store();
	~tri_list_store();

	void init(geom_parameters* geom_param_ptr);
	void add_tri(const int& tri_id, line_store* edge1, line_store* edge2, line_store* edge3);
	tri_store* get_triangle(const int& tri_id);

	void set_buffer();
	void paint_triangles();
	void clear_triangles();
	void update_opengl_uniforms(bool set_modelmatrix, bool set_pantranslation, bool set_rotatetranslation,
		bool set_zoomtranslation, bool set_transparency);
private:
	gBuffers tri_buffer;
	Shader tri_shader;

	void get_tri_buffer(tri_store& tri, float* tri_vertices, unsigned int& tri_v_index, unsigned int* tri_vertex_indices, unsigned int& tri_i_index);


};