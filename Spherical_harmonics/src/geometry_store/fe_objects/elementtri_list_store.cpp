#include "elementtri_list_store.h"

elementtri_list_store::elementtri_list_store()
{
	// Empty constructor
}

elementtri_list_store::~elementtri_list_store()
{
	// Empty destructor
}

void elementtri_list_store::init(geom_parameters* geom_param_ptr)
{
	// Set the geometry parameters
	this->geom_param_ptr = geom_param_ptr;

	// Set the geometry parameters for the labels (and clear the labels)
	element_tris.init(geom_param_ptr);
	element_tris_shrunk.init(geom_param_ptr);
	selected_element_tris_shrunk.init(geom_param_ptr);

	// Clear the triangles
	elementtri_count = 0;
	elementtriMap.clear();
}

void elementtri_list_store::add_elementtriangle(int& tri_id, node_store* nd1, node_store* nd2, node_store* nd3)
{
	// Add the Triangle to the list
	elementtri_store temp_tri;
	temp_tri.tri_id = tri_id; // Triangle ID
	temp_tri.nd1 = nd1;
	temp_tri.nd2 = nd2;
	temp_tri.nd3 = nd3;

	// Check whether the node_id is already there
	if (elementtriMap.find(tri_id) != elementtriMap.end())
	{
		// Element ID already exist (do not add)
		return;
	}

	// Insert to the triangles
	elementtriMap.insert({ tri_id, temp_tri });
	elementtri_count++;

	//__________________________ Add the Triangle
	glm::vec3 node_pt1 = (*nd1).node_pt;
	glm::vec3 node_pt2 = (*nd2).node_pt;
	glm::vec3 node_pt3 = (*nd3).node_pt;

	glm::vec3 temp_tri_color = geom_param_ptr->geom_colors.triangle_color;
	// Main triangle
	element_tris.add_tri(tri_id, node_pt1, node_pt2, node_pt3,
										temp_tri_color, temp_tri_color, temp_tri_color);

	// Main triangle as shrunk
	glm::vec3 midpt = glm::vec3((node_pt1.x + node_pt2.x + node_pt3.x) / 3.0f, // mid pt x
		(node_pt1.y + node_pt2.y + node_pt3.y) / 3.0f,  // mid pt y
		(node_pt1.z + node_pt2.z + node_pt3.z) / 3.0f); // mid pt z
	double shrink_factor = geom_param_ptr->traingle_shrunk_factor;

	glm::vec3 shrunk_node_pt1 = geom_parameters::linear_interpolation3d(midpt, node_pt1, shrink_factor);
	glm::vec3 shrunk_node_pt2 = geom_parameters::linear_interpolation3d(midpt, node_pt1, shrink_factor);
	glm::vec3 shrunk_node_pt3 = geom_parameters::linear_interpolation3d(midpt, node_pt1, shrink_factor);

	element_tris_shrunk.add_tri(tri_id, shrunk_node_pt1, shrunk_node_pt2, shrunk_node_pt3,
											temp_tri_color, temp_tri_color, temp_tri_color);

}

void elementtri_list_store::add_selection_triangles(const std::vector<int>& selected_element_ids)
{
	// Clear the existing selected triangles
	selected_element_tris_shrunk.clear_triangles();

	// Add to Selected Element triangles
	glm::vec3 temp_color = geom_param_ptr->geom_colors.selection_color;

	for (const auto& it : selected_element_ids)
	{
		int tri_id = elementtriMap[it].tri_id;
		glm::vec3 node_pt1 = elementtriMap[it].nd1->node_pt; // Node pt 1
		glm::vec3 node_pt2 = elementtriMap[it].nd2->node_pt; // Node pt 2
		glm::vec3 node_pt3 = elementtriMap[it].nd3->node_pt; // Node pt 3


		glm::vec3 midpt = glm::vec3((node_pt1.x + node_pt2.x + node_pt3.x) / 3.0f, // mid pt x
			(node_pt1.y + node_pt2.y + node_pt3.y) / 3.0f,  // mid pt y
			(node_pt1.z + node_pt2.z + node_pt3.z) / 3.0f); // mid pt z
		double shrink_factor = geom_param_ptr->traingle_shrunk_factor;

		glm::vec3 shrunk_node_pt1 = geom_parameters::linear_interpolation3d(midpt, node_pt1, shrink_factor);
		glm::vec3 shrunk_node_pt2 = geom_parameters::linear_interpolation3d(midpt, node_pt2, shrink_factor);
		glm::vec3 shrunk_node_pt3 = geom_parameters::linear_interpolation3d(midpt, node_pt3, shrink_factor);

		selected_element_tris_shrunk.add_tri(tri_id, shrunk_node_pt1, shrunk_node_pt2, shrunk_node_pt3,
														temp_color, temp_color, temp_color);
	}

	// Set the selected element triangles buffer
	selected_element_tris_shrunk.set_buffer();

}

void elementtri_list_store::set_buffer()
{
	// Set the buffers for the Model
	element_tris.set_buffer();
	element_tris_shrunk.set_buffer();
}

void elementtri_list_store::paint_elementtriangles()
{
	// Paint the triangles
	element_tris.paint_triangles();
}

void elementtri_list_store::paint_selected_elementtriangles()
{
	// Paint the selected triangles
	selected_element_tris_shrunk.paint_triangles();
}

void elementtri_list_store::paint_elementtriangles_shrunk()
{
	// Paint the triangles shrunk
	element_tris_shrunk.paint_triangles();
}

std::vector<int> elementtri_list_store::is_tri_selected(const glm::vec2& corner_pt1, const glm::vec2& corner_pt2)
{
	// Return the node id of node which is inside the rectangle
	// Covert mouse location to screen location
	int max_dim = geom_param_ptr->window_width > geom_param_ptr->window_height ? geom_param_ptr->window_width : geom_param_ptr->window_height;

	// Selected triangle list index;
	std::vector<int> selected_tri_index;

	// Transform the mouse location to openGL screen coordinates
	// Corner Point 1
	glm::vec2 screen_cpt1 = glm::vec2(2.0f * ((corner_pt1.x - (geom_param_ptr->window_width * 0.5f)) / max_dim),
		2.0f * (((geom_param_ptr->window_height * 0.5f) - corner_pt1.y) / max_dim));

	// Corner Point 2
	glm::vec2 screen_cpt2 = glm::vec2(2.0f * ((corner_pt2.x - (geom_param_ptr->window_width * 0.5f)) / max_dim),
		2.0f * (((geom_param_ptr->window_height * 0.5f) - corner_pt2.y) / max_dim));

	// Nodal location
	glm::mat4 scaling_matrix = glm::mat4(1.0) * static_cast<float>(geom_param_ptr->zoom_scale);
	scaling_matrix[3][3] = 1.0f;

	glm::mat4 scaledModelMatrix = scaling_matrix * geom_param_ptr->modelMatrix;

	// Loop through all triangles in map
	for (auto it = elementtriMap.begin(); it != elementtriMap.end(); ++it)
	{
		const glm::vec2& node_pt1 = it->second.nd1->node_pt;
		const glm::vec2& node_pt2 = it->second.nd2->node_pt;
		const glm::vec2& node_pt3 = it->second.nd3->node_pt;
		glm::vec2 md_pt_12 = geom_param_ptr->linear_interpolation(node_pt1, node_pt2, 0.50);
		glm::vec2 md_pt_23 = geom_param_ptr->linear_interpolation(node_pt2, node_pt3, 0.50);
		glm::vec2 md_pt_31 = geom_param_ptr->linear_interpolation(node_pt3, node_pt1, 0.50);
		glm::vec2 tri_midpt = glm::vec2((node_pt1.x + node_pt2.x + node_pt3.x) * 0.33f, (node_pt1.y + node_pt2.y + node_pt3.y) * 0.33f);

		//______________________________
		glm::vec4 node_pt1_fp = scaledModelMatrix * glm::vec4(node_pt1.x, node_pt1.y, 0, 1.0f) * geom_param_ptr->panTranslation;
		glm::vec4 node_pt2_fp = scaledModelMatrix * glm::vec4(node_pt2.x, node_pt2.y, 0, 1.0f) * geom_param_ptr->panTranslation;
		glm::vec4 node_pt3_fp = scaledModelMatrix * glm::vec4(node_pt3.x, node_pt3.y, 0, 1.0f) * geom_param_ptr->panTranslation;
		glm::vec4 md_pt_12_fp = scaledModelMatrix * glm::vec4(md_pt_12.x, md_pt_12.y, 0, 1.0f) * geom_param_ptr->panTranslation;
		glm::vec4 md_pt_23_fp = scaledModelMatrix * glm::vec4(md_pt_23.x, md_pt_23.y, 0, 1.0f) * geom_param_ptr->panTranslation;
		glm::vec4 md_pt_31_fp = scaledModelMatrix * glm::vec4(md_pt_31.x, md_pt_31.y, 0, 1.0f) * geom_param_ptr->panTranslation;
		glm::vec2 tri_midpt_fp = scaledModelMatrix * glm::vec4(tri_midpt.x, tri_midpt.y, 0, 1.0f) * geom_param_ptr->panTranslation;

		// Check whether the point inside a rectangle
		if (geom_param_ptr->isPointInsideRectangle(screen_cpt1, screen_cpt2, node_pt1_fp) == true ||
			geom_param_ptr->isPointInsideRectangle(screen_cpt1, screen_cpt2, node_pt2_fp) == true ||
			geom_param_ptr->isPointInsideRectangle(screen_cpt1, screen_cpt2, node_pt3_fp) == true ||
			geom_param_ptr->isPointInsideRectangle(screen_cpt1, screen_cpt2, md_pt_12_fp) == true ||
			geom_param_ptr->isPointInsideRectangle(screen_cpt1, screen_cpt2, md_pt_23_fp) == true ||
			geom_param_ptr->isPointInsideRectangle(screen_cpt1, screen_cpt2, md_pt_31_fp) == true ||
			geom_param_ptr->isPointInsideRectangle(screen_cpt1, screen_cpt2, tri_midpt_fp) == true)
		{
			selected_tri_index.push_back(it->first);
		}
	}

	// Return the tri index find
	return selected_tri_index;

}

void elementtri_list_store::update_geometry_matrices(bool set_modelmatrix, bool set_pantranslation, bool set_rotatetranslation,
	bool set_zoomtranslation, bool set_transparency, bool set_deflscale)
{
	// Update model openGL uniforms
	element_tris.update_opengl_uniforms(set_modelmatrix, set_pantranslation,set_rotatetranslation, set_zoomtranslation, set_transparency, set_deflscale);
	element_tris_shrunk.update_opengl_uniforms(set_modelmatrix, set_pantranslation, set_rotatetranslation, set_zoomtranslation, set_transparency, set_deflscale);
	selected_element_tris_shrunk.update_opengl_uniforms(set_modelmatrix, set_pantranslation, set_rotatetranslation, set_zoomtranslation, set_transparency, set_deflscale);
}

