#pragma once
#include "../geometry_buffers/gBuffers.h"
#include "../geom_parameters.h"
#include "point_list_store.h"
#include "line_list_store.h"
#include "tri_list_store.h"

class dcel_mesh_data
{
public:
	point_list_store node_points;
	point_list_store selected_node_points;
	line_list_store mesh_boundaries;
	line_list_store mesh_normals;
	tri_list_store element_tris;
	tri_list_store element_quads123;
	tri_list_store element_quads341;

	dcel_mesh_data();
	~dcel_mesh_data();
	void init(geom_parameters* geom_param_ptr);

	void add_mesh_point(const int& point_id,
		const double& x_coord,
		const double& y_coord,
		const double& z_coord);

	void add_selected_points(const std::vector<int>& selected_points);

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
	void paint_selected_points();
	void paint_mesh_normals();
	void paint_mesh_edges();
	void paint_triangles();
	void paint_quadrilaterals();

	void update_opengl_uniforms(bool set_modelmatrix, bool set_pantranslation, bool set_rotatetranslation,
		bool set_zoomtranslation, bool set_transparency);

private:
	int half_edge_count = 0;
	std::vector<line_store*> mesh_half_edges; // All the Half edge data

	int add_half_edge(const int& startPt_id, const int& endPt_id);

	void add_triangle(const int& tri_id,
		const int& point_id1,
		const int& point_id2,
		const int& point_id3,
		tri_list_store* triangle);

	void set_mesh_edge(line_store* edge,int& line_id,std::set<std::pair<int, int>>& seenLines);

	void set_mesh_normal(tri_store* tri);
};
