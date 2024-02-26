#include "dynamic_quad_list_store.h"

dynamic_quad_list_store::dynamic_quad_list_store()
{
	// Empty constructor
}

dynamic_quad_list_store::~dynamic_quad_list_store()
{
	// Empty destructor
}

void dynamic_quad_list_store::init(geom_parameters* geom_param_ptr)
{
	// Set the geometry parameters
	this->geom_param_ptr = geom_param_ptr;

	// Create the point shader
	std::filesystem::path shadersPath = geom_param_ptr->resourcePath;

	dyn_quad_shader.create_shader((shadersPath.string() + "/resources/shaders/mesh_dynvert_shader.vert").c_str(),
		(shadersPath.string() + "/resources/shaders/mesh_dynfrag_shader.frag").c_str());

	// Delete all the quadrilaterals
	clear_quadrilaterals();

}

void dynamic_quad_list_store::add_quad(const int& quad_id,
	dynamic_line_store* edge1, dynamic_line_store* edge2,
	dynamic_line_store* edge3, dynamic_line_store* edge4,
	dynamic_line_store* edge5, dynamic_line_store* edge6)
{
	// Add to the list
	dynamic_quad_store* temp_quad = new dynamic_quad_store;

	temp_quad->quad_id = quad_id;

	//___________________________________________________________________________________________
	// Half triangle 1
	dynamic_tri_store* temp_tri123 = new dynamic_tri_store;

	temp_tri123->tri_id = quad_id;
	temp_tri123->edge1 = edge1;
	temp_tri123->edge2 = edge2;
	temp_tri123->edge3 = edge3;

	// Set the face 123 normal
	temp_tri123->face_normal = geom_param_ptr->get_face_normal(edge1->start_pt->point_loc,
		edge2->start_pt->point_loc,
		edge3->start_pt->point_loc);

	temp_quad->tri123 = temp_tri123;

	//___________________________________________________________________________________________
	// Half triangle 2
	dynamic_tri_store* temp_tri341 = new dynamic_tri_store;

	temp_tri341->tri_id = quad_id;
	temp_tri341->edge1 = edge4;
	temp_tri341->edge2 = edge5;
	temp_tri341->edge3 = edge6;

	// Set the face 341 normal
	temp_tri341->face_normal = geom_param_ptr->get_face_normal(edge4->start_pt->point_loc,
		edge5->start_pt->point_loc,
		edge6->start_pt->point_loc);


	temp_quad->tri341 = temp_tri341;

	// Set the normal of the Quadrilateral
	temp_quad->face_normal = glm::normalize(temp_tri123->face_normal + temp_tri341->face_normal);


	// Add to the quad list
	dyn_quadMap.push_back(temp_quad);

	// Add to the quad id map
	dyn_quadId_Map.insert({ quad_id, dyn_quad_count });

	// Iterate the quad count
	dyn_quad_count++;

}

dynamic_tri_store* dynamic_quad_list_store::get_quadrilateral_face123(const int& quad_id)
{
	// Check whether quad_id exists?
	auto it = dyn_quadId_Map.find(quad_id);

	if (it != dyn_quadId_Map.end())
	{
		// quad id exists
		// return the address of the triangle 123
		return dyn_quadMap[it->second]->tri123;
	}
	else
	{
		// id not found
		return nullptr;
	}
}

dynamic_tri_store* dynamic_quad_list_store::get_quadrilateral_face341(const int& quad_id)
{
	// Check whether quad_id exists?
	auto it = dyn_quadId_Map.find(quad_id);

	if (it != dyn_quadId_Map.end())
	{
		// quad id exists
		// return the address of the triangle 341
		return dyn_quadMap[it->second]->tri341;
	}
	else
	{
		// id not found
		return nullptr;
	}
}

void dynamic_quad_list_store::set_buffer()
{
	// Set the buffer for index
	unsigned int quad_indices_count = 6 * dyn_quad_count; // 6 indices to form a quadrilateral
	unsigned int* quad_vertex_indices = new unsigned int[quad_indices_count];

	unsigned int quad_i_index = 0;

	// Set the node vertices
	for (auto& quad : dyn_quadMap)
	{
		// Add tri buffers
		get_quad_index_buffer(quad_vertex_indices, quad_i_index);
	}

	VertexBufferLayout quad_pt_layout;
	quad_pt_layout.AddFloat(3);  // Node center
	quad_pt_layout.AddFloat(3);  // Node normal
	quad_pt_layout.AddFloat(3);  // Node offset
	quad_pt_layout.AddFloat(1);  // Defl


	// Define the node vertices of the model for a node (3 position, 3 face normal, 3 defl, 1 defl mag ) 
	const unsigned int quad_vertex_count = 10 * 4 * dyn_quad_count;
	unsigned int quad_vertex_size = quad_vertex_count * sizeof(float); // Size of the node_vertex

	// Create the Node Deflection buffers
	dyn_quad_buffer.CreateDynamicBuffers(quad_vertex_size, quad_vertex_indices, quad_indices_count, quad_pt_layout);

	// Delete the dynamic array
	delete[] quad_vertex_indices;
}

void dynamic_quad_list_store::paint_quadrilaterals()
{
	// Paint all the Quadrilaterals
	dyn_quad_shader.Bind();
	dyn_quad_buffer.Bind();

	// Quad buffer updated seperately for dynamic drawing (To draw cyclic data ex. modal vectors)

	glDrawElements(GL_TRIANGLES, (6 * dyn_quad_count), GL_UNSIGNED_INT, 0);
	dyn_quad_buffer.UnBind();
	dyn_quad_shader.UnBind();

}


void dynamic_quad_list_store::update_buffer(const int& dyn_index)
{
	// Define the node vertices of the model for a node (3 position, 3 face normal, 3 defl, 1 defl mag) 
	const unsigned int quad_vertex_count = 10 * 4 * dyn_quad_count;
	float* quad_vertices = new float[quad_vertex_count];

	unsigned int quad_v_index = 0;

	// Set the quad point vertices
	for (auto& quad : dyn_quadMap)
	{
		// Add points buffers
		get_quad_vertex_buffer(quad, dyn_index, quad_vertices, quad_v_index);
	}

	unsigned int quad_vertex_size = quad_vertex_count * sizeof(float); // Size of the quad point vertex

	// Update the buffer
	dyn_quad_buffer.UpdateDynamicVertexBuffer(quad_vertices, quad_vertex_size);

	// Delete the dynamic array
	delete[] quad_vertices;

}

void dynamic_quad_list_store::clear_quadrilaterals()
{
	// Clear the quadrilaterals
	dyn_quad_count = 0;

	//// Delete dynamically allocated memory pointed to by the struct members
	//for (auto& quad : dyn_quadMap)
	//{
	//	delete quad->tri123;
	//	delete quad->tri341;
	//}

	dyn_quadId_Map.clear();
	dyn_quadMap.clear();
}

void dynamic_quad_list_store::update_opengl_uniforms(bool set_modelmatrix, bool set_pantranslation,
	bool set_rotatetranslation, bool set_zoomtranslation, bool set_transparency, bool set_deflscale)
{
	if (set_modelmatrix == true)
	{
		// set the model matrix
		dyn_quad_shader.setUniform("geom_scale", static_cast<float>(geom_param_ptr->geom_scale));
		dyn_quad_shader.setUniform("transparency", 0.7f);

		dyn_quad_shader.setUniform("modelMatrix", geom_param_ptr->modelMatrix, false);
	}

	if (set_pantranslation == true)
	{
		// set the pan translation
		dyn_quad_shader.setUniform("panTranslation", geom_param_ptr->panTranslation, false);
	}

	if (set_rotatetranslation == true)
	{
		// set the rotate translation
		dyn_quad_shader.setUniform("rotateTranslation", geom_param_ptr->rotateTranslation, false);
	}

	if (set_zoomtranslation == true)
	{
		// set the zoom translation
		dyn_quad_shader.setUniform("zoomscale", static_cast<float>(geom_param_ptr->zoom_scale));
	}

	if (set_transparency == true)
	{
		// set the alpha transparency
		// dyn_quad_shader.setUniform("transparency", static_cast<float>(geom_param_ptr->geom_transparency));
	}

	if (set_deflscale == true)
	{
		// set the deflection scale
		dyn_quad_shader.setUniform("normalized_deflscale", static_cast<float>(geom_param_ptr->normalized_defl_scale));
		dyn_quad_shader.setUniform("deflscale", static_cast<float>(geom_param_ptr->defl_scale));
	}


}

void dynamic_quad_list_store::get_quad_vertex_buffer(dynamic_quad_store* quad, const int& dyn_index,
	float* dyn_quad_vertices, unsigned int& dyn_quad_v_index)
{
	// Get the node buffer for the shader
	// Quad Point 1
	// Point location
	dyn_quad_vertices[dyn_quad_v_index + 0] = quad->tri123->edge1->start_pt->point_loc.x;
	dyn_quad_vertices[dyn_quad_v_index + 1] = quad->tri123->edge1->start_pt->point_loc.y;
	dyn_quad_vertices[dyn_quad_v_index + 2] = quad->tri123->edge1->start_pt->point_loc.z;

	// Point normal
	dyn_quad_vertices[dyn_quad_v_index + 3] = quad->face_normal.x;
	dyn_quad_vertices[dyn_quad_v_index + 4] = quad->face_normal.y;
	dyn_quad_vertices[dyn_quad_v_index + 5] = quad->face_normal.z;

	// Point offset
	dyn_quad_vertices[dyn_quad_v_index + 6] = quad->tri123->edge1->start_pt->point_offset[dyn_index].x;
	dyn_quad_vertices[dyn_quad_v_index + 7] = quad->tri123->edge1->start_pt->point_offset[dyn_index].y;
	dyn_quad_vertices[dyn_quad_v_index + 8] = quad->tri123->edge1->start_pt->point_offset[dyn_index].z;

	// Normalized deflection value
	dyn_quad_vertices[dyn_quad_v_index + 9] = quad->tri123->edge1->start_pt->point_offset_mag[dyn_index];

	// Iterate
	dyn_quad_v_index = dyn_quad_v_index + 10;

	// Quad Point 2
	// Point location
	dyn_quad_vertices[dyn_quad_v_index + 0] = quad->tri123->edge2->start_pt->point_loc.x;
	dyn_quad_vertices[dyn_quad_v_index + 1] = quad->tri123->edge2->start_pt->point_loc.y;
	dyn_quad_vertices[dyn_quad_v_index + 2] = quad->tri123->edge2->start_pt->point_loc.z;

	// Point normal
	dyn_quad_vertices[dyn_quad_v_index + 3] = quad->face_normal.x;
	dyn_quad_vertices[dyn_quad_v_index + 4] = quad->face_normal.y;
	dyn_quad_vertices[dyn_quad_v_index + 5] = quad->face_normal.z;

	// Point offset
	dyn_quad_vertices[dyn_quad_v_index + 6] = quad->tri123->edge2->start_pt->point_offset[dyn_index].x;
	dyn_quad_vertices[dyn_quad_v_index + 7] = quad->tri123->edge2->start_pt->point_offset[dyn_index].y;
	dyn_quad_vertices[dyn_quad_v_index + 8] = quad->tri123->edge2->start_pt->point_offset[dyn_index].z;

	// Normalized deflection value
	dyn_quad_vertices[dyn_quad_v_index + 9] = quad->tri123->edge2->start_pt->point_offset_mag[dyn_index];

	// Iterate
	dyn_quad_v_index = dyn_quad_v_index + 10;


	// Quad Point 3
	// Point location
	dyn_quad_vertices[dyn_quad_v_index + 0] = quad->tri341->edge1->start_pt->point_loc.x;
	dyn_quad_vertices[dyn_quad_v_index + 1] = quad->tri341->edge1->start_pt->point_loc.y;
	dyn_quad_vertices[dyn_quad_v_index + 2] = quad->tri341->edge1->start_pt->point_loc.z;

	// Point normal
	dyn_quad_vertices[dyn_quad_v_index + 3] = quad->face_normal.x;
	dyn_quad_vertices[dyn_quad_v_index + 4] = quad->face_normal.y;
	dyn_quad_vertices[dyn_quad_v_index + 5] = quad->face_normal.z;

	// Point offset
	dyn_quad_vertices[dyn_quad_v_index + 6] = quad->tri341->edge1->start_pt->point_offset[dyn_index].x;
	dyn_quad_vertices[dyn_quad_v_index + 7] = quad->tri341->edge1->start_pt->point_offset[dyn_index].y;
	dyn_quad_vertices[dyn_quad_v_index + 8] = quad->tri341->edge1->start_pt->point_offset[dyn_index].z;

	// Normalized deflection value
	dyn_quad_vertices[dyn_quad_v_index + 9] = quad->tri341->edge1->start_pt->point_offset_mag[dyn_index];

	// Iterate
	dyn_quad_v_index = dyn_quad_v_index + 10;


	// Quad Point 4
	// Point location
	dyn_quad_vertices[dyn_quad_v_index + 0] = quad->tri341->edge2->start_pt->point_loc.x;
	dyn_quad_vertices[dyn_quad_v_index + 1] = quad->tri341->edge2->start_pt->point_loc.y;
	dyn_quad_vertices[dyn_quad_v_index + 2] = quad->tri341->edge2->start_pt->point_loc.z;

	// Point normal
	dyn_quad_vertices[dyn_quad_v_index + 3] = quad->face_normal.x;
	dyn_quad_vertices[dyn_quad_v_index + 4] = quad->face_normal.y;
	dyn_quad_vertices[dyn_quad_v_index + 5] = quad->face_normal.z;

	// Point offset
	dyn_quad_vertices[dyn_quad_v_index + 6] = quad->tri341->edge2->start_pt->point_offset[dyn_index].x;
	dyn_quad_vertices[dyn_quad_v_index + 7] = quad->tri341->edge2->start_pt->point_offset[dyn_index].y;
	dyn_quad_vertices[dyn_quad_v_index + 8] = quad->tri341->edge2->start_pt->point_offset[dyn_index].z;

	// Normalized deflection value
	dyn_quad_vertices[dyn_quad_v_index + 9] = quad->tri341->edge2->start_pt->point_offset_mag[dyn_index];

	// Iterate
	dyn_quad_v_index = dyn_quad_v_index + 10;
}

void dynamic_quad_list_store::get_quad_index_buffer(unsigned int* dyn_quad_vertex_indices, unsigned int& dyn_quad_i_index)
{
	//__________________________________________________________________________
	// Add the indices
	// Index 0 1 2 
	dyn_quad_vertex_indices[dyn_quad_i_index + 0] = static_cast<int>((dyn_quad_i_index / 6.0) * 4.0) + 0;

	dyn_quad_vertex_indices[dyn_quad_i_index + 1] = static_cast<int>((dyn_quad_i_index / 6.0) * 4.0) + 1;

	dyn_quad_vertex_indices[dyn_quad_i_index + 2] = static_cast<int>((dyn_quad_i_index / 6.0) * 4.0) + 2;

	// Index 2 3 0 
	dyn_quad_vertex_indices[dyn_quad_i_index + 3] = static_cast<int>((dyn_quad_i_index / 6.0) * 4.0) + 2;

	dyn_quad_vertex_indices[dyn_quad_i_index + 4] = static_cast<int>((dyn_quad_i_index / 6.0) * 4.0) + 3;

	dyn_quad_vertex_indices[dyn_quad_i_index + 5] = static_cast<int>((dyn_quad_i_index / 6.0) * 4.0) + 0;

	// Iterate
	dyn_quad_i_index = dyn_quad_i_index + 6;

}
