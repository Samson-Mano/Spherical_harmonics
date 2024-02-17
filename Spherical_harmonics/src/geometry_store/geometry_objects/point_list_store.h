#pragma once
#include "../geometry_buffers/gBuffers.h"
#include "../geom_parameters.h"

struct point_store
{
	// store the individual point
	int point_id = -1; // Point ID
	double x_coord = 0.0; // x coordinate
	double y_coord = 0.0; // y coordinate
	double z_coord = 0.0; // z coordinate
};

struct tri_store; // Forward declaration

struct line_store
{
	// store the individual point
	int line_id = -1;

	point_store* origin = nullptr;
	point_store* destination = nullptr;
	line_store* next = nullptr;        // Next half-edge in the same face
	line_store* twin = nullptr;        // Opposite half-edge
	tri_store* face = nullptr;    // Face to the left of this half-edge
};


struct tri_store
{
	// store the individual point
	int tri_id = -1;

	// Edges
	line_store* edge1 = nullptr; // Triangle edge 1
	line_store* edge2 = nullptr; // Triangle edge 2
	line_store* edge3 = nullptr; // Triangle edge 3
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
	void set_buffer();
	void paint_points();
	void clear_points();
	void update_opengl_uniforms(bool set_modelmatrix, bool set_pantranslation, bool set_rotatetranslation,
		bool set_zoomtranslation, bool set_transparency, bool set_deflscale);
private:
	gBuffers point_buffer;
	Shader point_shader;

	void get_node_buffer(point_store& pt,float* point_vertices, unsigned int& point_v_index, unsigned int* point_indices, unsigned int& point_i_index);
};



