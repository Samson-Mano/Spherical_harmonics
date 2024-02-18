#include "dcel_mesh_data.h"

dcel_mesh_data::dcel_mesh_data()
{
	// Empty constructor
}

dcel_mesh_data::~dcel_mesh_data()
{
	// Empty destructor
}

void dcel_mesh_data::init(geom_parameters* geom_param_ptr)
{
	// Initialize the geometry objects of the mesh
	node_points.init(geom_param_ptr);
	selected_node_points.init(geom_param_ptr);
	mesh_half_edges.init(geom_param_ptr);
	mesh_boundaries.init(geom_param_ptr);
	element_tris.init(geom_param_ptr);
	element_quads123.init(geom_param_ptr);
	element_quads341.init(geom_param_ptr);
}

void dcel_mesh_data::add_mesh_point(const int& point_id, const double& x_coord, const double& y_coord, const double& z_coord)
{
	// Add the node points
	node_points.add_point(point_id, x_coord, y_coord, z_coord);
}

void dcel_mesh_data::add_selected_points(const std::vector<int>& selected_points)
{

}

void dcel_mesh_data::add_mesh_triangle(const int& tri_id, const int& point_id1, const int& point_id2, const int& point_id3)
{
	//    2____3 
	//    |   /  
	//    | /    
	//    1      

	add_triangle(tri_id, point_id1, point_id2, point_id3, element_tris);

}

void dcel_mesh_data::add_mesh_quadrilateral(const int& quad_id,	const int& point_id1,	const int& point_id2,	const int& point_id3,	const int& point_id4)
{
	//   2______3     2____3      3
	//   |      |     |   /     / |
	//   |      |     | /     /   | 
	//   1______4     1      1____4    

	// Add half triangle 1
	add_triangle(quad_id, point_id1, point_id2, point_id3, element_quads123);

	// Add hald triangle 2
	add_triangle(quad_id, point_id3, point_id4, point_id1, element_quads341);

}


void dcel_mesh_data::add_triangle(const int& tri_id, const int& point_id1,	const int& point_id2, const int& point_id3,
	tri_list_store& triangle)
{
	// Add the half triangle of the quadrilaterals
	// Add three half edges
	int line_id1, line_id2, line_id3;

	// Add edge 1
	line_id1 = mesh_half_edges.line_count;
	mesh_half_edges.add_line(line_id1,
		node_points.get_point(point_id1),
		node_points.get_point(point_id2));

	// Add edge 2
	line_id2 = mesh_half_edges.line_count;
	mesh_half_edges.add_line(line_id2,
		node_points.get_point(point_id2),
		node_points.get_point(point_id3));

	// Add edge 3
	line_id3 = mesh_half_edges.line_count;
	mesh_half_edges.add_line(line_id3,
		node_points.get_point(point_id3),
		node_points.get_point(point_id1));

	//________________________________________
	// Add the mesh triangles
	line_store*  edge_1 = mesh_half_edges.get_line(line_id1);
	line_store*  edge_2 = mesh_half_edges.get_line(line_id2);
	line_store*  edge_3 = mesh_half_edges.get_line(line_id3);

	triangle.add_tri(tri_id, mesh_half_edges.get_line(line_id1),
		mesh_half_edges.get_line(line_id2), 
		mesh_half_edges.get_line(line_id3));


	//// Update the Half edge pointers
	//// Update the Halfedge's next Halfedge pointer
	//edge_1->next_line = edge_2;
	//edge_2->next_line = edge_3;
	//edge_3->next_line = edge_1;

	//// Update the Halfedge's triangle face pointer
	//tri_store* tri = triangle.get_triangle(tri_id);

	//edge_1->face = tri;
	//edge_2->face = tri;
	//edge_3->face = tri;
}


void dcel_mesh_data::set_mesh_wireframe()
{
	// std::vector<elementline_store> uniqueLines;
	mesh_boundaries.clear_lines();

	std::set<std::pair<int, int>> seenLines;
	std::pair<int, int> lineEndpoint_ids;
	int smaller_id = 0;
	int larger_id = 0;
	int line_id = 0;

	// Triangles
	for (const auto& tri : element_tris.triMap)
	{
		// Edge 1
		line_store* edge1 = tri.edge1;

		// Ensure the start point ID is smaller than the end point ID
		smaller_id = std::min(edge1->start_pt->point_id, edge1->end_pt->point_id);
		larger_id = std::max(edge1->start_pt->point_id, edge1->end_pt->point_id);
		lineEndpoint_ids = std::make_pair(smaller_id, larger_id);

		// If the line is not already seen in the opposite direction, add it to the unique lines
		if (seenLines.find(lineEndpoint_ids) == seenLines.end())
		{
			// Create the mesh boundary
			mesh_boundaries.add_line(line_id,
				node_points.get_point(smaller_id),
				node_points.get_point(larger_id));

			line_id++;

			// Add to the seen lines
			seenLines.insert(lineEndpoint_ids);
		}


		// Edge 2
		line_store* edge2 = tri.edge2;

		// Ensure the start point ID is smaller than the end point ID
		smaller_id = std::min(edge2->start_pt->point_id, edge2->end_pt->point_id);
		larger_id = std::max(edge2->start_pt->point_id, edge2->end_pt->point_id);
		lineEndpoint_ids = std::make_pair(smaller_id, larger_id);

		// If the line is not already seen in the opposite direction, add it to the unique lines
		if (seenLines.find(lineEndpoint_ids) == seenLines.end())
		{
			// Create the mesh boundary
			mesh_boundaries.add_line(line_id,
				node_points.get_point(smaller_id),
				node_points.get_point(larger_id));

			line_id++;

			// Add to the seen lines
			seenLines.insert(lineEndpoint_ids);
		}


		// Edge 3
		line_store* edge3 = tri.edge3;

		// Ensure the start point ID is smaller than the end point ID
		smaller_id = std::min(edge3->start_pt->point_id, edge3->end_pt->point_id);
		larger_id = std::max(edge3->start_pt->point_id, edge3->end_pt->point_id);
		lineEndpoint_ids = std::make_pair(smaller_id, larger_id);

		// If the line is not already seen in the opposite direction, add it to the unique lines
		if (seenLines.find(lineEndpoint_ids) == seenLines.end())
		{
			// Create the mesh boundary
			mesh_boundaries.add_line(line_id,
				node_points.get_point(smaller_id),
				node_points.get_point(larger_id));

			line_id++;

			// Add to the seen lines
			seenLines.insert(lineEndpoint_ids);
		}
		
	}

	// Quadrilaterals
	for (const auto& tri : element_quads123.triMap)
	{
		// Edge 1
		line_store* edge1 = tri.edge1;

		// Ensure the start point ID is smaller than the end point ID
		smaller_id = std::min(edge1->start_pt->point_id, edge1->end_pt->point_id);
		larger_id = std::max(edge1->start_pt->point_id, edge1->end_pt->point_id);
		lineEndpoint_ids = std::make_pair(smaller_id, larger_id);

		// If the line is not already seen in the opposite direction, add it to the unique lines
		if (seenLines.find(lineEndpoint_ids) == seenLines.end())
		{
			// Create the mesh boundary
			mesh_boundaries.add_line(line_id,
				node_points.get_point(smaller_id),
				node_points.get_point(larger_id));

			line_id++;

			// Add to the seen lines
			seenLines.insert(lineEndpoint_ids);
		}


		// Edge 2
		line_store* edge2 = tri.edge2;

		// Ensure the start point ID is smaller than the end point ID
		smaller_id = std::min(edge2->start_pt->point_id, edge2->end_pt->point_id);
		larger_id = std::max(edge2->start_pt->point_id, edge2->end_pt->point_id);
		lineEndpoint_ids = std::make_pair(smaller_id, larger_id);

		// If the line is not already seen in the opposite direction, add it to the unique lines
		if (seenLines.find(lineEndpoint_ids) == seenLines.end())
		{
			// Create the mesh boundary
			mesh_boundaries.add_line(line_id,
				node_points.get_point(smaller_id),
				node_points.get_point(larger_id));

			line_id++;

			// Add to the seen lines
			seenLines.insert(lineEndpoint_ids);
		}


		// Edge 3
		// Do Not Add - because its quadrialterals diagonals
	}


	for (const auto& tri : element_quads341.triMap)
	{
		// Edge 1
		line_store* edge1 = tri.edge1;

		// Ensure the start point ID is smaller than the end point ID
		smaller_id = std::min(edge1->start_pt->point_id, edge1->end_pt->point_id);
		larger_id = std::max(edge1->start_pt->point_id, edge1->end_pt->point_id);
		lineEndpoint_ids = std::make_pair(smaller_id, larger_id);

		// If the line is not already seen in the opposite direction, add it to the unique lines
		if (seenLines.find(lineEndpoint_ids) == seenLines.end())
		{
			// Create the mesh boundary
			mesh_boundaries.add_line(line_id,
				node_points.get_point(smaller_id),
				node_points.get_point(larger_id));

			line_id++;

			// Add to the seen lines
			seenLines.insert(lineEndpoint_ids);
		}


		// Edge 2
		line_store* edge2 = tri.edge2;

		// Ensure the start point ID is smaller than the end point ID
		smaller_id = std::min(edge2->start_pt->point_id, edge2->end_pt->point_id);
		larger_id = std::max(edge2->start_pt->point_id, edge2->end_pt->point_id);
		lineEndpoint_ids = std::make_pair(smaller_id, larger_id);

		// If the line is not already seen in the opposite direction, add it to the unique lines
		if (seenLines.find(lineEndpoint_ids) == seenLines.end())
		{
			// Create the mesh boundary
			mesh_boundaries.add_line(line_id,
				node_points.get_point(smaller_id),
				node_points.get_point(larger_id));

			line_id++;

			// Add to the seen lines
			seenLines.insert(lineEndpoint_ids);
		}


		// Edge 3
		// Do Not Add - because its quadrialterals diagonals
	}

}


void dcel_mesh_data::clear_mesh()
{
	// Clear the mesh
	node_points.clear_points();
	selected_node_points.clear_points();
	mesh_half_edges.clear_lines();
	mesh_boundaries.clear_lines();
	element_tris.clear_triangles();
	element_quads123.clear_triangles(); 
	element_quads341.clear_triangles(); 

}

void dcel_mesh_data::set_buffer()
{
	// Set the buffer
	node_points.set_buffer();
	mesh_boundaries.set_buffer();
	element_tris.set_buffer();
	element_quads123.set_buffer(); 
	element_quads341.set_buffer(); 

}

void dcel_mesh_data::paint_points()
{
	// Paint the nodal points
	node_points.paint_points();

}

void dcel_mesh_data::paint_selected_points()
{
	// Paint the selected nodal points
	selected_node_points.paint_points();

}

void dcel_mesh_data::paint_mesh_edges()
{
	// Paint the mesh boundaries
	mesh_boundaries.paint_lines();

}

void dcel_mesh_data::paint_triangles()
{
	// Paint the triangles
	element_tris.paint_triangles();

}

void dcel_mesh_data::paint_quadrilaterals()
{
	// Paint the quadrilaterals
	element_quads123.paint_triangles(); // Triangle with edge 123
	element_quads341.paint_triangles(); // Triangle with edge 341

}


void dcel_mesh_data::update_opengl_uniforms(bool set_modelmatrix, bool set_pantranslation, bool set_rotatetranslation,
	bool set_zoomtranslation, bool set_transparency)
{
	// Update the openGL uniform values of geometry
	node_points.update_opengl_uniforms(set_modelmatrix, set_pantranslation, set_rotatetranslation, set_zoomtranslation, set_transparency);
	selected_node_points.update_opengl_uniforms(set_modelmatrix, set_pantranslation, set_rotatetranslation, set_zoomtranslation, set_transparency);
	mesh_boundaries.update_opengl_uniforms(set_modelmatrix, set_pantranslation, set_rotatetranslation, set_zoomtranslation, set_transparency);
	element_tris.update_opengl_uniforms(set_modelmatrix, set_pantranslation, set_rotatetranslation, set_zoomtranslation, set_transparency);
	element_quads123.update_opengl_uniforms(set_modelmatrix, set_pantranslation, set_rotatetranslation, set_zoomtranslation, set_transparency);
	element_quads341.update_opengl_uniforms(set_modelmatrix, set_pantranslation, set_rotatetranslation, set_zoomtranslation, set_transparency);

}
