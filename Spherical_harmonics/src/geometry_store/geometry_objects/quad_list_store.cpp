#include "quad_list_store.h"

quad_list_store::quad_list_store()
{
	// Empty constructor
}

quad_list_store::~quad_list_store()
{
	// Empty destructor
}

void quad_list_store::init(geom_parameters* geom_param_ptr)
{
	// Set the geometry parameters
	this->geom_param_ptr = geom_param_ptr;

	// Create the quadrilateral shader
	std::filesystem::path shadersPath = geom_param_ptr->resourcePath;

	quad_shader.create_shader((shadersPath.string() + "/resources/shaders/mesh_vert_shader.vert").c_str(),
		(shadersPath.string() + "/resources/shaders/mesh_frag_shader.frag").c_str());

	quad_shader.setUniform("ptColor", geom_param_ptr->geom_colors.triangle_color);

	// Delete all the quadrilaterals
	clear_quadrilaterals();
}

void quad_list_store::add_quad(const int& quad_id, line_store* edge1, line_store* edge2, line_store* edge3, line_store* edge4)
{
	// Add to the list
	quad_store* temp_quad = new quad_store;

	temp_quad->quad_id = quad_id;

	//___________________________________________________________________________________________
	// Half triangle 1
	tri_store* temp_tri123 = new tri_store;

	temp_tri123->tri_id = quad_id;
	temp_tri123->edge1 = edge1;
	temp_tri123->edge2 = edge2;
	temp_tri123->edge3 = edge3;

	// Find the normal of the face
	temp_tri123->face_normal = geom_param_ptr->get_face_normal(edge1->start_pt->pt_coord(),
		edge2->start_pt->pt_coord(),
		edge3->start_pt->pt_coord());

	// Find the geometric center of the face
	temp_tri123->geom_center = geom_param_ptr->findGeometricCenter(edge1->start_pt->pt_coord(),
		edge2->start_pt->pt_coord(),
		edge3->start_pt->pt_coord());

	temp_quad->tri123 = temp_tri123;

	//___________________________________________________________________________________________
	// Half triangle 2
	tri_store* temp_tri341 = new tri_store;

	temp_tri341->tri_id = quad_id;
	temp_tri341->edge1 = edge3;
	temp_tri341->edge2 = edge4;
	temp_tri341->edge3 = edge1;

	// Find the normal of the face
	temp_tri341->face_normal = geom_param_ptr->get_face_normal(edge3->start_pt->pt_coord(),
		edge4->start_pt->pt_coord(),
		edge1->start_pt->pt_coord());

	// Find the geometric center of the face
	temp_tri341->geom_center = geom_param_ptr->findGeometricCenter(edge3->start_pt->pt_coord(),
		edge4->start_pt->pt_coord(),
		edge1->start_pt->pt_coord());

	temp_quad->tri341 = temp_tri341;

	// Find the normal of the face
	temp_quad->face_normal = glm::normalize(temp_tri123->face_normal + temp_tri341->face_normal);

	// Find the geometric center of the face
	temp_quad->geom_center = (temp_tri123->geom_center + temp_tri341->geom_center) * 0.5f;


	quadMap.push_back(temp_quad);

	// Add to the quad id map
	quadId_Map.insert({ quad_id, quad_count });

	// Iterate the quad count
	quad_count++;
}

tri_store* quad_list_store::get_quadrilateral_face123(const int& quad_id)
{
	// Check whether quad_id exists?
	auto it = quadId_Map.find(quad_id);

	if (it != quadId_Map.end())
	{
		// quad id exists
		// return the address of the triangle
		return quadMap[it->second]->tri123;
	}
	else
	{
		// id not found
		return nullptr;
	}
}

tri_store* quad_list_store::get_quadrilateral_face341(const int& quad_id)
{
	// Check whether quad_id exists?
	auto it = quadId_Map.find(quad_id);

	if (it != quadId_Map.end())
	{
		// quad id exists
		// return the address of the triangle
		return quadMap[it->second]->tri341;
	}
	else
	{
		// id not found
		return nullptr;
	}
}


void quad_list_store::set_buffer()
{
	// Define the quad vertices of the model for a node (3 position & 3 normal) 4 points
	const unsigned int quad_vertex_count = 6 * 4 * quad_count;
	float* quad_vertices = new float[quad_vertex_count];

	unsigned int quad_indices_count = 6 * quad_count; // 6 indices to form a quadrilateral ( 3 + 3 triangles)
	unsigned int* quad_vertex_indices = new unsigned int[quad_indices_count];

	unsigned int quad_v_index = 0;
	unsigned int quad_i_index = 0;

	// Set the quad vertices
	for (auto& quad : quadMap)
	{
		// Add quadrilateral buffers
		get_quad_buffer(quad, quad_vertices, quad_v_index, quad_vertex_indices, quad_i_index);
	}

	VertexBufferLayout quad_pt_layout;
	quad_pt_layout.AddFloat(3);  // Node center
	quad_pt_layout.AddFloat(3);  // Node normal

	unsigned int quad_vertex_size = quad_vertex_count * sizeof(float); // Size of the node_vertex

	// Create the quadrilateral buffers
	quad_buffer.CreateBuffers(quad_vertices, quad_vertex_size, quad_vertex_indices, quad_indices_count, quad_pt_layout);

	// Delete the dynamic array
	delete[] quad_vertices;
	delete[] quad_vertex_indices;
}

void quad_list_store::paint_quadrilaterals()
{
	// Paint all the triangles
	quad_shader.Bind();
	quad_buffer.Bind();
	glDrawElements(GL_TRIANGLES, (6 * quad_count), GL_UNSIGNED_INT, 0);
	quad_buffer.UnBind();
	quad_shader.UnBind();
}

void quad_list_store::clear_quadrilaterals()
{
	// Delete all the quadrilaterals
	quad_count = 0;
	quadMap.clear();
	quadId_Map.clear();
}

void quad_list_store::update_opengl_uniforms(bool set_modelmatrix, bool set_pantranslation, bool set_rotatetranslation, bool set_zoomtranslation, bool set_transparency)
{
	if (set_modelmatrix == true)
	{
		// set the model matrix
		// quad_shader.setUniform("geom_scale", static_cast<float>(geom_param_ptr->geom_scale));
		quad_shader.setUniform("transparency", 1.0f);

		// quad_shader.setUniform("projectionMatrix", geom_param_ptr->projectionMatrix, false);
		// quad_shader.setUniform("viewMatrix", geom_param_ptr->viewMatrix, false);
		quad_shader.setUniform("modelMatrix", geom_param_ptr->modelMatrix, false);
	}

	if (set_pantranslation == true)
	{
		// set the pan translation
		quad_shader.setUniform("panTranslation", geom_param_ptr->panTranslation, false);
	}

	if (set_rotatetranslation == true)
	{
		// set the rotate translation
		quad_shader.setUniform("rotateTranslation", geom_param_ptr->rotateTranslation, false);
	}

	if (set_zoomtranslation == true)
	{
		// set the zoom translation
		quad_shader.setUniform("zoomscale", static_cast<float>(geom_param_ptr->zoom_scale));
	}

	if (set_transparency == true)
	{
		// set the alpha transparency
		quad_shader.setUniform("transparency", static_cast<float>(geom_param_ptr->geom_transparency));
	}
}

void quad_list_store::get_quad_buffer(quad_store* quad, float* quad_vertices, unsigned int& quad_v_index, unsigned int* quad_vertex_indices, unsigned int& quad_i_index)
{
	// Get the three node buffer for the shader
	// Point 1
	// Point location
	quad_vertices[quad_v_index + 0] = quad->tri123->edge1->start_pt->x_coord;
	quad_vertices[quad_v_index + 1] = quad->tri123->edge1->start_pt->y_coord;
	quad_vertices[quad_v_index + 2] = quad->tri123->edge1->start_pt->z_coord;

	// Point normal
	quad_vertices[quad_v_index + 3] = quad->face_normal.x;
	quad_vertices[quad_v_index + 4] = quad->face_normal.y;
	quad_vertices[quad_v_index + 5] = quad->face_normal.z;

	// Iterate
	quad_v_index = quad_v_index + 6;

	// Point 2
	// Point location
	quad_vertices[quad_v_index + 0] = quad->tri123->edge2->start_pt->x_coord;
	quad_vertices[quad_v_index + 1] = quad->tri123->edge2->start_pt->y_coord;
	quad_vertices[quad_v_index + 2] = quad->tri123->edge2->start_pt->z_coord;

	// Point normal
	quad_vertices[quad_v_index + 3] = quad->face_normal.x;
	quad_vertices[quad_v_index + 4] = quad->face_normal.y;
	quad_vertices[quad_v_index + 5] = quad->face_normal.z;

	// Iterate
	quad_v_index = quad_v_index + 6;

	// Point 3
	// Point location
	quad_vertices[quad_v_index + 0] = quad->tri341->edge1->start_pt->x_coord;
	quad_vertices[quad_v_index + 1] = quad->tri341->edge1->start_pt->y_coord;
	quad_vertices[quad_v_index + 2] = quad->tri341->edge1->start_pt->z_coord;

	// Point normal
	quad_vertices[quad_v_index + 3] = quad->face_normal.x;
	quad_vertices[quad_v_index + 4] = quad->face_normal.y;
	quad_vertices[quad_v_index + 5] = quad->face_normal.z;

	// Iterate
	quad_v_index = quad_v_index + 6;

	// Point 4
	// Point location
	quad_vertices[quad_v_index + 0] = quad->tri341->edge2->start_pt->x_coord;
	quad_vertices[quad_v_index + 1] = quad->tri341->edge2->start_pt->y_coord;
	quad_vertices[quad_v_index + 2] = quad->tri341->edge2->start_pt->z_coord;

	// Point normal
	quad_vertices[quad_v_index + 3] = quad->face_normal.x;
	quad_vertices[quad_v_index + 4] = quad->face_normal.y;
	quad_vertices[quad_v_index + 5] = quad->face_normal.z;

	// Iterate
	quad_v_index = quad_v_index + 6;

	//__________________________________________________________________________
	// Add the indices
	// Index 0 1 2 
	quad_vertex_indices[quad_i_index + 0] =	static_cast<int>((quad_i_index / 6.0) * 4.0) + 0;

	quad_vertex_indices[quad_i_index + 1] = static_cast<int>((quad_i_index / 6.0) * 4.0) + 1;

	quad_vertex_indices[quad_i_index + 2] = static_cast<int>((quad_i_index / 6.0) * 4.0) + 2;

	// Index 2 3 0 
	quad_vertex_indices[quad_i_index + 3] = static_cast<int>((quad_i_index / 6.0) * 4.0) + 2;

	quad_vertex_indices[quad_i_index + 4] = static_cast<int>((quad_i_index / 6.0) * 4.0) + 3;

	quad_vertex_indices[quad_i_index + 5] = static_cast<int>((quad_i_index / 6.0) * 4.0) + 0;

	// Iterate
	quad_i_index = quad_i_index + 6;


}
