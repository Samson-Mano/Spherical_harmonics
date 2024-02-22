#include "dcel_dynmesh_data.h"

dcel_dynmesh_data::dcel_dynmesh_data()
{
	// Empty constructor
}

dcel_dynmesh_data::~dcel_dynmesh_data()
{
	// Empty destructor
}

void dcel_dynmesh_data::init(geom_parameters* geom_param_ptr)
{

	// Initialize the geometry objects of the mesh
	half_edge_count = 0;
	mesh_half_edges.clear(); // clear the half edge data

	// Nodes
	node_points.init(geom_param_ptr);

	// Mesh boundaries & mesh normals
	mesh_normals.init(geom_param_ptr);
	mesh_boundaries.init(geom_param_ptr);

	// Mesh data
	element_tris.init(geom_param_ptr);
	element_quads.init(geom_param_ptr);

}

void dcel_dynmesh_data::add_mesh_point(const int& point_id, const glm::vec3 point_loc, 
	const std::vector<glm::vec3> point_offset, const std::vector<double> point_offset_mag)
{
	// Add the node points
	node_points.add_point(point_id, point_loc,point_offset, point_offset_mag);

}

void dcel_dynmesh_data::add_mesh_triangle(const int& tri_id, const int& point_id1, const int& point_id2, const int& point_id3)
{
	//    2____3 
//    |   /  
//    | /    
//    1      

// Add the half triangle of the quadrilaterals
// Add three half edges
	int line_id1, line_id2, line_id3;

	// Add edge 1
	line_id1 = add_half_edge(point_id1, point_id2);

	// Add edge 2
	line_id2 = add_half_edge(point_id2, point_id3);

	// Add edge 3
	line_id3 = add_half_edge(point_id3, point_id1);


	//________________________________________
	// Add the mesh triangles
	element_tris.add_tri(tri_id, mesh_half_edges[line_id1],
		mesh_half_edges[line_id2],
		mesh_half_edges[line_id3]);


	// Set the half edges next line
	mesh_half_edges[line_id1]->next_line = mesh_half_edges[line_id2];
	mesh_half_edges[line_id2]->next_line = mesh_half_edges[line_id3];
	mesh_half_edges[line_id3]->next_line = mesh_half_edges[line_id1];

	// Set the half edge face data
	dynamic_tri_store* temp_tri = element_tris.get_triangle(tri_id);

	mesh_half_edges[line_id1]->face = temp_tri;
	mesh_half_edges[line_id2]->face = temp_tri;
	mesh_half_edges[line_id3]->face = temp_tri;


}

void dcel_dynmesh_data::add_mesh_quadrilateral(const int& quad_id, const int& point_id1, const int& point_id2, const int& point_id3, const int& point_id4)
{
	//   2______3     2____3      3
	//   |      |     |   /     / |
	//   |      |     | /     /   | 
	//   1______4     1      1____4    

	// Add the quadrilaterals
	// Add the 1st half triangle of the quadrilaterals
	// Add three half edges
	int line_id1, line_id2, line_id3;

	// Add edge 1
	line_id1 = add_half_edge(point_id1, point_id2);

	// Add edge 2
	line_id2 = add_half_edge(point_id2, point_id3);

	// Add edge 3
	line_id3 = add_half_edge(point_id3, point_id1);

	// Set the half edges next line
	mesh_half_edges[line_id1]->next_line = mesh_half_edges[line_id2];
	mesh_half_edges[line_id2]->next_line = mesh_half_edges[line_id3];
	mesh_half_edges[line_id3]->next_line = mesh_half_edges[line_id1];


	// Add the 2nd half triangle of the quadrilaterals
	// Add three half edges
	int line_id4, line_id5, line_id6;

	// Add edge 4
	line_id4 = add_half_edge(point_id3, point_id4);

	// Add edge 5
	line_id5 = add_half_edge(point_id4, point_id1);

	// Add edge 6
	line_id6 = add_half_edge(point_id1, point_id3);


	// Set the half edges next line
	mesh_half_edges[line_id4]->next_line = mesh_half_edges[line_id5];
	mesh_half_edges[line_id5]->next_line = mesh_half_edges[line_id6];
	mesh_half_edges[line_id6]->next_line = mesh_half_edges[line_id4];


	//________________________________________
	// Add the mesh quadrilaterals
	element_quads.add_quad(quad_id, mesh_half_edges[line_id1],
		mesh_half_edges[line_id2],
		mesh_half_edges[line_id4],
		mesh_half_edges[line_id5]);

	// Set the half edge face data 1st Half triangle of the quadrilateral
	dynamic_tri_store* temp_tri123 = element_quads.get_quadrilateral_face123(quad_id);

	mesh_half_edges[line_id1]->face = temp_tri123;
	mesh_half_edges[line_id2]->face = temp_tri123;
	mesh_half_edges[line_id3]->face = temp_tri123;

	// Set the half edge face data 2nd Half triangle of the quadrilateral
	dynamic_tri_store* temp_tri341 = element_quads.get_quadrilateral_face341(quad_id);

	mesh_half_edges[line_id4]->face = temp_tri341;
	mesh_half_edges[line_id5]->face = temp_tri341;
	mesh_half_edges[line_id6]->face = temp_tri341;

}

void dcel_dynmesh_data::set_mesh_wireframe()
{
	// Step 1: Set the twin of half edges
	for (int i = 0; i < static_cast<int>(mesh_half_edges.size()); i++)
	{
		// If the twin_line is already set, continue to the next half-edge
		if (mesh_half_edges[i]->twin_line != nullptr)
			continue;

		// Get the start and end points of the current half-edge
		dynamic_point_store* start_pt = mesh_half_edges[i]->start_pt;
		dynamic_point_store* end_pt = mesh_half_edges[i]->end_pt;

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
	for (const auto& tri : element_tris.dyn_triMap)
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
	for (const auto& quad : element_quads.dyn_quadMap)
	{
		// Edge 1
		set_mesh_edge(quad->tri123->edge1, line_id, seenLines);

		// Edge 2
		set_mesh_edge(quad->tri123->edge2, line_id, seenLines);

		// Edge 3
		set_mesh_edge(quad->tri341->edge1, line_id, seenLines);

		// Edge 4
		set_mesh_edge(quad->tri341->edge2, line_id, seenLines);

		// Set the mesh normal
		set_mesh_normal(quad);
	}

}

void dcel_dynmesh_data::clear_mesh()
{
	// Clear the mesh
	// Half edge
	half_edge_count = 0;
	mesh_half_edges.clear(); // clear the half edges

	// Nodes
	node_points.clear_points();

	// Mesh boundaries & mesh normals
	mesh_normals.clear_lines();
	mesh_boundaries.clear_lines();

	// Mesh data
	element_tris.clear_triangles();
	element_quads.clear_quadrilaterals();

}

void dcel_dynmesh_data::set_buffer()
{
	// Set the buffer
	node_points.set_buffer();
	mesh_normals.set_buffer();
	mesh_boundaries.set_buffer();
	element_tris.set_buffer();
	element_quads.set_buffer();

}

void dcel_dynmesh_data::paint_points()
{
	// Paint the nodal points
	node_points.paint_points();

}

void dcel_dynmesh_data::paint_mesh_normals()
{
	// Paint the mesh normals
	mesh_normals.paint_lines();

}

void dcel_dynmesh_data::paint_mesh_edges()
{
	// Paint the mesh boundaries
	mesh_boundaries.paint_lines();

}

void dcel_dynmesh_data::paint_triangles()
{
	// Paint the triangles
	element_tris.paint_triangles();

}

void dcel_dynmesh_data::paint_quadrilaterals()
{
	// Paint the quadrilaterals
	element_quads.paint_quadrilaterals();

}

void dcel_dynmesh_data::update_opengl_uniforms(bool set_modelmatrix, bool set_pantranslation,
	bool set_rotatetranslation, bool set_zoomtranslation, bool set_transparency,bool set_deflscale)
{
	// Update the openGL uniform values of geometry
	node_points.update_opengl_uniforms(set_modelmatrix, set_pantranslation, set_rotatetranslation, set_zoomtranslation, set_transparency, set_deflscale);
	mesh_normals.update_opengl_uniforms(set_modelmatrix, set_pantranslation, set_rotatetranslation, set_zoomtranslation, set_transparency, set_deflscale);
	mesh_boundaries.update_opengl_uniforms(set_modelmatrix, set_pantranslation, set_rotatetranslation, set_zoomtranslation, set_transparency, set_deflscale);
	element_tris.update_opengl_uniforms(set_modelmatrix, set_pantranslation, set_rotatetranslation, set_zoomtranslation, set_transparency, set_deflscale);
	element_quads.update_opengl_uniforms(set_modelmatrix, set_pantranslation, set_rotatetranslation, set_zoomtranslation, set_transparency, set_deflscale);

}

int dcel_dynmesh_data::add_half_edge(const int& startPt_id, const int& endPt_id)
{
	// Add the Half edge
	dynamic_line_store* temp_edge = new dynamic_line_store;
	temp_edge->line_id = half_edge_count;
	temp_edge->start_pt = node_points.get_point(startPt_id);
	temp_edge->end_pt = node_points.get_point(endPt_id);

	// Add to the Half edge list
	mesh_half_edges.push_back(temp_edge);

	// Iterate
	half_edge_count++;

	return (half_edge_count - 1); // return the index of last addition
}

void dcel_dynmesh_data::set_mesh_edge(dynamic_line_store* edge, int& line_id, std::set<std::pair<int, int>>& seenLines)
{
	// Ensure the start point ID is smaller than the end point ID
	int smaller_id = std::min(edge->start_pt->point_id, edge->end_pt->point_id);
	int larger_id = std::max(edge->start_pt->point_id, edge->end_pt->point_id);
	std::pair<int, int> lineEndpoint_ids = std::make_pair(smaller_id, larger_id);

	// If the line is not already seen in the opposite direction, add it to the unique lines
	if (seenLines.find(lineEndpoint_ids) == seenLines.end())
	{
		// line normal list
		std::vector<glm::vec3> line_normal;

		if (edge->twin_line != nullptr)
		{
			for (int i = 0; i< static_cast<int> (edge->face->face_normal.size()); i++)
			{
				// get the light and right face normal
				glm::vec3 left_face_normal = edge->face->face_normal[i];

				glm::vec3 right_face_normal = edge->twin_line->face->face_normal[i];

				// Compute the average normal only if twin_line is not nullptr
				line_normal.push_back(glm::normalize(left_face_normal + right_face_normal));

			}
		
		}
		else
		{
			for (int i = 0; i < static_cast<int> (edge->face->face_normal.size()); i++)
			{
				// Handle the case where twin_line is nullptr
				line_normal.push_back(glm::normalize(edge->face->face_normal[i]));
			}
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

void dcel_dynmesh_data::set_mesh_normal(dynamic_tri_store* tri)
{
}

void dcel_dynmesh_data::set_mesh_normal(dynamic_quad_store* quad)
{
}
