#pragma once
#include "../geometry_buffers/gBuffers.h"
#include "../geom_parameters.h"
#include "dynamic_point_list_store.h"
#include "dynamic_line_list_store.h"
#include "dynamic_tri_list_store.h"
#include "dynamic_quad_list_store.h"


class dcel_dynmesh_data
{
public:
	dynamic_point_list_store node_points;
	dynamic_line_list_store mesh_boundaries;
	dynamic_line_list_store mesh_normals;
	dynamic_tri_list_store element_tris;
	dynamic_quad_list_store element_quads;


	dcel_dynmesh_data();
	~dcel_dynmesh_data();
	void init(geom_parameters* geom_param_ptr);

	void add_mesh_point(const int& point_id,
		const glm::vec3 point_loc,
		const std::vector<glm::vec3> point_offset,
		const std::vector<double> point_offset_mag);

	void add_mesh_triangle(const int& tri_id,
		const int& point_id1,
		const int& point_id2,
		const int& point_id3);

	void add_mesh_quadrilateral(const int& quad_id,
		const int& point_id1,
		const int& point_id2,
		const int& point_id3,
		const int& point_id4);

	void set_mesh_wireframe();

	void clear_mesh();

	void set_buffer();

	void paint_points();
	void paint_mesh_normals();
	void paint_mesh_edges();
	void paint_triangles();
	void paint_quadrilaterals();

	void update_buffer(const int& draw_step);

	void update_opengl_uniforms(bool set_modelmatrix, bool set_pantranslation, bool set_rotatetranslation,
		bool set_zoomtranslation, bool set_transparency, bool set_deflscale);

private:
	int half_edge_count = 0;
	std::vector<dynamic_line_store*> mesh_half_edges; // All the Half edge data

	int add_half_edge(const int& startPt_id, const int& endPt_id);

	void set_mesh_edge(dynamic_line_store* edge, int& line_id, std::set<std::pair<int, int>>& seenLines);

};
