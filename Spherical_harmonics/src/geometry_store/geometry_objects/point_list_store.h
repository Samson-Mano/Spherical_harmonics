#pragma once
#include "../geometry_buffers/gBuffers.h"
#include "../geom_parameters.h"

struct tri_store; // forward declaration

struct point_store
{
	// store the individual point
	int point_id = -1; // Point ID
	double x_coord = 0.0; // x coordinate
	double y_coord = 0.0; // y coordinate
	double z_coord = 0.0; // z coordinate

	glm::vec3 pt_coord() const
	{
		return glm::vec3(x_coord, y_coord, z_coord);
	}
};

class point_list_store
{
	// Store all the points
public:
	geom_parameters* geom_param_ptr = nullptr;
	unsigned int point_count = 0;
	std::unordered_map<int, int> pointId_Map;
	std::vector<point_store> pointMap;

	point_list_store();
	~point_list_store();
	void init(geom_parameters* geom_param_ptr);
	void add_point(const int& point_id, const double& x_coord, const double& y_coord, const double& z_coord );
	point_store* get_point(const int& point_id);

	void set_buffer();
	void paint_points();
	void clear_points();
	void update_opengl_uniforms(bool set_modelmatrix, bool set_pantranslation, bool set_rotatetranslation,
		bool set_zoomtranslation, bool set_transparency);
private:
	gBuffers point_buffer;
	Shader point_shader;

	void get_node_buffer(point_store& pt,float* point_vertices, unsigned int& point_v_index, unsigned int* point_indices, unsigned int& point_i_index);
};



