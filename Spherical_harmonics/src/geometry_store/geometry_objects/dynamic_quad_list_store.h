#pragma once
#include "dynamic_tri_list_store.h"

struct dynamic_quad_store
{
	// store the individual Quad
	int quad_id = -1;

	// Half triangles of quadrilaterals
	dynamic_tri_store* tri123 = nullptr;
	dynamic_tri_store* tri341 = nullptr;

	// face normal
	glm::vec3 face_normal = glm::vec3(0); // Face normal of the quadrilateral

};


class dynamic_quad_list_store
{
public:
	geom_parameters* geom_param_ptr = nullptr;
	unsigned int dyn_quad_count = 0;
	std::unordered_map<int, int> dyn_quadId_Map;
	std::vector<dynamic_quad_store*> dyn_quadMap;

	dynamic_quad_list_store();
	~dynamic_quad_list_store();

	void init(geom_parameters* geom_param_ptr);
	void add_quad(const int& quad_id, dynamic_line_store* edge1, dynamic_line_store* edge2,
		dynamic_line_store* edge3, dynamic_line_store* edge4,
		dynamic_line_store* edge5, dynamic_line_store* edge6);
	dynamic_tri_store* get_quadrilateral_face123(const int& quad_id);
	dynamic_tri_store* get_quadrilateral_face341(const int& quad_id);


	void set_buffer();
	void paint_quadrilaterals();
	void update_buffer(const int& dyn_index);
	void clear_quadrilaterals();
	void update_opengl_uniforms(bool set_modelmatrix, bool set_pantranslation, bool set_rotatetranslation,
		bool set_zoomtranslation, bool set_transparency, bool set_deflscale);
private:
	gBuffers dyn_quad_buffer;
	Shader dyn_quad_shader;

	void get_quad_vertex_buffer(dynamic_quad_store* quad, const int& dyn_index,
		float* dyn_quad_vertices, unsigned int& dyn_quad_v_index);

	void get_quad_index_buffer(unsigned int* dyn_quad_vertex_indices, unsigned int& dyn_quad_i_index);
};
