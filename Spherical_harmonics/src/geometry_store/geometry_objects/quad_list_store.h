#pragma once
#include "tri_list_store.h"

struct quad_store
{
	// store the individual Quad
	int quad_id = -1;

	// Half triangles of quadrilaterals
	tri_store* tri123 = nullptr;
	tri_store* tri341 = nullptr;

	// face normal
	glm::vec3 face_normal = glm::vec3(0); // Face normal of the quadrilateral
	glm::vec3 geom_center = glm::vec3(0); // Geometric center of the quadrilateral
};



class quad_list_store
{
public:
	geom_parameters* geom_param_ptr = nullptr;
	unsigned int quad_count = 0;
	std::unordered_map<int, int> quadId_Map;
	std::vector<quad_store*> quadMap;


	quad_list_store();
	~quad_list_store();

	void init(geom_parameters* geom_param_ptr);
	void add_quad(const int& quad_id, line_store* edge1, line_store* edge2, line_store* edge3, line_store* edge4);
	tri_store* get_quadrilateral_face123(const int& quad_id);
	tri_store* get_quadrilateral_face341(const int& quad_id);

	void set_buffer();
	void paint_quadrilaterals();
	void clear_quadrilaterals();
	void update_opengl_uniforms(bool set_modelmatrix, bool set_pantranslation, bool set_rotatetranslation,
		bool set_zoomtranslation, bool set_transparency);
private:
	gBuffers quad_buffer;
	Shader quad_shader;

	void get_quad_buffer(quad_store* quad, float* quad_vertices, unsigned int& quad_v_index, unsigned int* quad_vertex_indices, unsigned int& quad_i_index);


};
