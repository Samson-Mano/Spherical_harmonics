#include "tri_list_store.h"

tri_list_store::tri_list_store()
{
	// Empty constructor
}

tri_list_store::~tri_list_store()
{
	// Empty destructor
}

void tri_list_store::init(geom_parameters* geom_param_ptr)
{
	// Set the geometry parameters
	this->geom_param_ptr = geom_param_ptr;

	// Create the point shader
	std::filesystem::path shadersPath = geom_param_ptr->resourcePath;

	tri_shader.create_shader((shadersPath.string() + "/resources/shaders/mesh_vert_shader.vert").c_str(),
		(shadersPath.string() + "/resources/shaders/mesh_frag_shader.frag").c_str());

	tri_shader.setUniform("ptColor", geom_param_ptr->geom_colors.triangle_color);

	// Delete all the triangles
	clear_triangles();
}

void tri_list_store::add_tri(const int& tri_id, line_store* edge1, line_store* edge2, line_store* edge3)
{
	// Add to the list
	tri_store* temp_tri = new tri_store;

	temp_tri->tri_id = tri_id;
	temp_tri->edge1 = edge1;
	temp_tri->edge2 = edge2;
	temp_tri->edge3 = edge3;

	// Find the normal of the face
	temp_tri->face_normal = geom_param_ptr->get_face_normal(edge1->start_pt->pt_coord(),
		edge2->start_pt->pt_coord(),
		edge3->start_pt->pt_coord());


	triMap.push_back(temp_tri);

	// Add to the tri id map
	triId_Map.insert({ tri_id, tri_count });

	// Iterate the point count
	tri_count++;
}


tri_store* tri_list_store::get_triangle(const int& tri_id)
{
	// Check whether tri_id exists?
	auto it = triId_Map.find(tri_id);

	if (it != triId_Map.end())
	{
		// tri id exists
		// return the address of the triangle
		return triMap[it->second];
	}
	else
	{
		// id not found
		return nullptr;
	}
}


void tri_list_store::set_buffer()
{
	// Define the tri vertices of the model for a node (3 position & 3 normal) 
	const unsigned int tri_vertex_count = 6 * 3 * tri_count;
	float* tri_vertices = new float[tri_vertex_count];

	unsigned int tri_indices_count = 3 * tri_count; // 3 indices to form a triangle
	unsigned int* tri_vertex_indices = new unsigned int[tri_indices_count];

	unsigned int tri_v_index = 0;
	unsigned int tri_i_index = 0;

	// Set the tri vertices
	for (auto& tri : triMap)
	{
		// Add triangle buffers
		get_tri_buffer(tri, tri_vertices, tri_v_index, tri_vertex_indices, tri_i_index);
	}

	VertexBufferLayout tri_pt_layout;
	tri_pt_layout.AddFloat(3);  // Node center
	tri_pt_layout.AddFloat(3);  // Node normal

	unsigned int tri_vertex_size = tri_vertex_count * sizeof(float); // Size of the node_vertex

	// Create the triangle buffers
	tri_buffer.CreateBuffers(tri_vertices, tri_vertex_size, tri_vertex_indices, tri_indices_count, tri_pt_layout);

	// Delete the dynamic array
	delete[] tri_vertices;
	delete[] tri_vertex_indices;
}

void tri_list_store::paint_triangles()
{
	// Paint all the triangles
	tri_shader.Bind();
	tri_buffer.Bind();
	glDrawElements(GL_TRIANGLES, (3 * tri_count), GL_UNSIGNED_INT, 0);
	tri_buffer.UnBind();
	tri_shader.UnBind();
}

void tri_list_store::clear_triangles()
{
	// Delete all the triangles
	tri_count = 0;
	triMap.clear();
	triId_Map.clear();
}

void tri_list_store::update_opengl_uniforms(bool set_modelmatrix, bool set_pantranslation, bool set_rotatetranslation, 
	bool set_zoomtranslation, bool set_transparency)
{
	if (set_modelmatrix == true)
	{
		// set the model matrix
		// tri_shader.setUniform("geom_scale", static_cast<float>(geom_param_ptr->geom_scale));
		tri_shader.setUniform("transparency", 1.0f);

		// tri_shader.setUniform("projectionMatrix", geom_param_ptr->projectionMatrix, false);
		// tri_shader.setUniform("viewMatrix", geom_param_ptr->viewMatrix, false);
		tri_shader.setUniform("modelMatrix", geom_param_ptr->modelMatrix, false);
	}

	if (set_pantranslation == true)
	{
		// set the pan translation
		tri_shader.setUniform("panTranslation", geom_param_ptr->panTranslation, false);
	}

	if (set_rotatetranslation == true)
	{
		// set the rotate translation
		tri_shader.setUniform("rotateTranslation", geom_param_ptr->rotateTranslation, false);
	}

	if (set_zoomtranslation == true)
	{
		// set the zoom translation
		tri_shader.setUniform("zoomscale", static_cast<float>(geom_param_ptr->zoom_scale));
	}

	if (set_transparency == true)
	{
		// set the alpha transparency
		tri_shader.setUniform("transparency", static_cast<float>(geom_param_ptr->geom_transparency));
	}
}

void tri_list_store::get_tri_buffer(tri_store* tri, float* tri_vertices, unsigned int& tri_v_index, unsigned int* tri_vertex_indices, unsigned int& tri_i_index)
{
	// Get the three node buffer for the shader
	// Point 1
	// Point location
	tri_vertices[tri_v_index + 0] = tri->edge1->start_pt->x_coord;
	tri_vertices[tri_v_index + 1] = tri->edge1->start_pt->y_coord;
	tri_vertices[tri_v_index + 2] = tri->edge1->start_pt->z_coord;

	// Point normal
	tri_vertices[tri_v_index + 3] = tri->face_normal.x;
	tri_vertices[tri_v_index + 4] = tri->face_normal.y;
	tri_vertices[tri_v_index + 5] = tri->face_normal.z;

	// Iterate
	tri_v_index = tri_v_index + 6;

	// Point 2
	// Point location
	tri_vertices[tri_v_index + 0] = tri->edge2->start_pt->x_coord;
	tri_vertices[tri_v_index + 1] = tri->edge2->start_pt->y_coord;
	tri_vertices[tri_v_index + 2] = tri->edge2->start_pt->z_coord;

	// Point normal
	tri_vertices[tri_v_index + 3] = tri->face_normal.x;
	tri_vertices[tri_v_index + 4] = tri->face_normal.y;
	tri_vertices[tri_v_index + 5] = tri->face_normal.z;

	// Iterate
	tri_v_index = tri_v_index + 6;

	// Point 3
	// Point location
	tri_vertices[tri_v_index + 0] = tri->edge3->start_pt->x_coord;
	tri_vertices[tri_v_index + 1] = tri->edge3->start_pt->y_coord;
	tri_vertices[tri_v_index + 2] = tri->edge3->start_pt->z_coord;

	// Point normal
	tri_vertices[tri_v_index + 3] = tri->face_normal.x;
	tri_vertices[tri_v_index + 4] = tri->face_normal.y;
	tri_vertices[tri_v_index + 5] = tri->face_normal.z;

	// Iterate
	tri_v_index = tri_v_index + 6;

	//__________________________________________________________________________
	// Add the indices
	// Index 1
	tri_vertex_indices[tri_i_index] = tri_i_index;

	tri_i_index = tri_i_index + 1;

	// Index 2
	tri_vertex_indices[tri_i_index] = tri_i_index;

	tri_i_index = tri_i_index + 1;

	// Index 3
	tri_vertex_indices[tri_i_index] = tri_i_index;

	tri_i_index = tri_i_index + 1;
}