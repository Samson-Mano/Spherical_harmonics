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
	half_edge_count = 0;
	mesh_half_edges.clear(); // clear the half edge data

	// Nodes
	node_points.init(geom_param_ptr);
	selected_node_points.init(geom_param_ptr);
	
	// Mesh boundaries & mesh normals
	mesh_normals.init(geom_param_ptr);
	mesh_boundaries.init(geom_param_ptr);

	// Mesh data
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

	add_triangle(tri_id, point_id1, point_id2, point_id3, &element_tris);

}

void dcel_mesh_data::add_mesh_quadrilateral(const int& quad_id,	const int& point_id1,	const int& point_id2,	const int& point_id3,	const int& point_id4)
{
	//   2______3     2____3      3
	//   |      |     |   /     / |
	//   |      |     | /     /   | 
	//   1______4     1      1____4    

	// Add the half triangle of the quadrilaterals
	
	// Add half triangle 1
	add_triangle(quad_id, point_id1, point_id2, point_id3, &element_quads123);

	// Add hald triangle 2
	add_triangle(quad_id, point_id3, point_id4, point_id1, &element_quads341);

}


void dcel_mesh_data::add_triangle(const int& tri_id, const int& point_id1,	const int& point_id2, const int& point_id3,
	tri_list_store* triangle)
{
	// Add the half triangle of the quadrilaterals
	// Add three half edges
	int line_id1, line_id2, line_id3;

	// Add edge 1
	line_id1 = add_half_edge(point_id1,point_id2);

	// Add edge 2
	line_id2 = add_half_edge(point_id2, point_id3);

	// Add edge 3
	line_id3 = add_half_edge(point_id3, point_id1);


	//________________________________________
	// Add the mesh triangles
	triangle->add_tri(tri_id, mesh_half_edges[line_id1],
		mesh_half_edges[line_id2],
		mesh_half_edges[line_id3]);


	// Set the half edges next line
	mesh_half_edges[line_id1]->next_line = mesh_half_edges[line_id2];
	mesh_half_edges[line_id2]->next_line = mesh_half_edges[line_id3];
	mesh_half_edges[line_id3]->next_line = mesh_half_edges[line_id1];

	// Set the half edge face data
	mesh_half_edges[line_id1]->face = triangle->get_triangle(tri_id);
	mesh_half_edges[line_id2]->face = triangle->get_triangle(tri_id);
	mesh_half_edges[line_id3]->face = triangle->get_triangle(tri_id);

}

int dcel_mesh_data::add_half_edge(const int& startPt_id, const int& endPt_id)
{
	// Add the Half edge
	line_store* temp_edge = new line_store;
	temp_edge->line_id = half_edge_count;
	temp_edge->start_pt = node_points.get_point(startPt_id);
	temp_edge->end_pt = node_points.get_point(endPt_id);

	// Add to the Half edge list
	mesh_half_edges.push_back(temp_edge);

	// Iterate
	half_edge_count++;

	return (half_edge_count - 1); // return the index of last addition
}


void dcel_mesh_data::set_mesh_wireframe()
{
	// Step 1: Set the twin of half edges
	for (int i = 0; i < static_cast<int>(mesh_half_edges.size()); i++)
	{
		// If the twin_line is already set, continue to the next half-edge
		if (mesh_half_edges[i]->twin_line != nullptr)
			continue;

		// Get the start and end points of the current half-edge
		point_store* start_pt = mesh_half_edges[i]->start_pt;
		point_store* end_pt = mesh_half_edges[i]->end_pt;

		// Iterate through the remaining half-edges to find the twin
		for (int j = i + 1; j < static_cast<int>(mesh_half_edges.size()); j++)
		{
			// Check if the current half-edge has the same start and end points as the twin
			if (mesh_half_edges[j]->start_pt == end_pt && mesh_half_edges[j]->end_pt == start_pt)
			{
				// Set the twin_line for both half-edges
				mesh_half_edges[i]->twin_line = mesh_half_edges[j];
				mesh_half_edges[j]->twin_line = mesh_half_edges[i];
				break; // Break the loop since twin has been found
			}
		}
	}

	//_________________________________________________________________________________________________
	// Step 2: Set the mesh boundaries & mesh normals
	mesh_boundaries.clear_lines();
	mesh_normals.clear_lines();

	std::set<std::pair<int, int>> seenLines;
	int line_id = 0;

	// Triangles
	for (const auto& tri : element_tris.triMap)
	{
		// Edge 1
		set_mesh_edge(tri->edge1, line_id, seenLines);

		// Edge 2
		set_mesh_edge(tri->edge2, line_id, seenLines);

		// Edge 3
		set_mesh_edge(tri->edge3, line_id, seenLines);

		// Set the mesh normal
		set_mesh_normal(tri);
	}

	// Quadrilaterals
	// Half triangle 1
	for (const auto& tri : element_quads123.triMap)
	{
		// Edge 1
		set_mesh_edge(tri->edge1, line_id, seenLines);

		// Edge 2
		set_mesh_edge(tri->edge2, line_id, seenLines);

		// Edge 3
		// Do Not Add - because its quadrialteral's diagonal

		// Set the mesh normal
		set_mesh_normal(tri);
	}

	// Half triangle 2
	for (const auto& tri : element_quads341.triMap)
	{
		// Edge 1
		set_mesh_edge(tri->edge1, line_id, seenLines);

		// Edge 2
		set_mesh_edge(tri->edge2, line_id, seenLines);

		// Edge 3
		// Do Not Add - because its quadrialteral's diagonal

		// Set the mesh normal
		set_mesh_normal(tri);
	}

}


void dcel_mesh_data::set_mesh_edge(line_store* edge,int& line_id, std::set<std::pair<int, int>>& seenLines)
{
	// Ensure the start point ID is smaller than the end point ID
	int smaller_id = std::min(edge->start_pt->point_id, edge->end_pt->point_id);
	int larger_id = std::max(edge->start_pt->point_id, edge->end_pt->point_id);
	std::pair<int, int> lineEndpoint_ids = std::make_pair(smaller_id, larger_id);

	// If the line is not already seen in the opposite direction, add it to the unique lines
	if (seenLines.find(lineEndpoint_ids) == seenLines.end())
	{
		glm::vec3 line_normal = glm::vec3(0);

		// get the light and right face normal
		glm::vec3 left_face_normal = edge->face->face_normal;

		if (edge->twin_line != nullptr)
		{
			glm::vec3 right_face_normal = edge->twin_line->face->face_normal;
			// Compute the average normal only if twin_line is not nullptr
			line_normal = glm::normalize(left_face_normal + right_face_normal);
		}
		else
		{
			// Handle the case where twin_line is nullptr
			line_normal = glm::normalize(left_face_normal);
		}

		edge->line_normal = line_normal;

		// Create the mesh boundary
		mesh_boundaries.add_line(line_id,
			node_points.get_point(smaller_id),
			node_points.get_point(larger_id), line_normal);

		line_id++;

		// Add to the seen lines
		seenLines.insert(lineEndpoint_ids);
	}
}

void dcel_mesh_data::set_mesh_normal(tri_store* tri)
{
	// Get the geometric center of the triangle
	// Function returns the geometric center of four nodes
	glm::vec3 face_center(0);

	face_center += tri->edge1->start_pt->pt_coord();
	face_center += tri->edge2->start_pt->pt_coord();
	face_center += tri->edge3->start_pt->pt_coord();
	
	face_center =  face_center / 3.0f;

	point_store* pt1 = new point_store;
	pt1->x_coord = face_center.x;
	pt1->y_coord = face_center.y;
	pt1->z_coord = face_center.z;

	point_store* pt2 = new point_store;
	pt2->x_coord = face_center.x + 10.0 * tri->face_normal.x;
	pt2->y_coord = face_center.y + 10.0 * tri->face_normal.y;
	pt2->z_coord = face_center.z + 10.0 * tri->face_normal.z;

	int normal_id = mesh_normals.line_count;
	mesh_normals.add_line(normal_id, pt1, pt2, tri->face_normal);

}


void dcel_mesh_data::clear_mesh()
{
	// Clear the mesh
	// Half edge
	half_edge_count = 0;
	mesh_half_edges.clear(); // clear the half edges

	// Nodes
	node_points.clear_points();
	selected_node_points.clear_points();

	// Mesh boundaries & mesh normals
	mesh_normals.clear_lines();
	mesh_boundaries.clear_lines();

	// Mesh data
	element_tris.clear_triangles();
	element_quads123.clear_triangles(); 
	element_quads341.clear_triangles(); 

}

void dcel_mesh_data::set_buffer()
{
	// Set the buffer
	node_points.set_buffer();
	mesh_normals.set_buffer();
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

void dcel_mesh_data::paint_mesh_normals()
{
	// Paint the mesh normals
	mesh_normals.paint_lines();

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
	mesh_normals.update_opengl_uniforms(set_modelmatrix, set_pantranslation, set_rotatetranslation, set_zoomtranslation, set_transparency);
	mesh_boundaries.update_opengl_uniforms(set_modelmatrix, set_pantranslation, set_rotatetranslation, set_zoomtranslation, set_transparency);
	element_tris.update_opengl_uniforms(set_modelmatrix, set_pantranslation, set_rotatetranslation, set_zoomtranslation, set_transparency);
	element_quads123.update_opengl_uniforms(set_modelmatrix, set_pantranslation, set_rotatetranslation, set_zoomtranslation, set_transparency);
	element_quads341.update_opengl_uniforms(set_modelmatrix, set_pantranslation, set_rotatetranslation, set_zoomtranslation, set_transparency);

}
