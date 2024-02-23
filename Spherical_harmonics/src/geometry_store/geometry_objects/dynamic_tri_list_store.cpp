#include "dynamic_tri_list_store.h"


dynamic_tri_list_store::dynamic_tri_list_store()
{
	// Empty constructor
}

dynamic_tri_list_store::~dynamic_tri_list_store()
{
	// Empty destructor
}

void dynamic_tri_list_store::init(geom_parameters* geom_param_ptr)
{
	// Set the geometry parameters
	this->geom_param_ptr = geom_param_ptr;

	// Create the point shader
	std::filesystem::path shadersPath = geom_param_ptr->resourcePath;

	dyn_tri_shader.create_shader((shadersPath.string() + "/resources/shaders/mesh_dynvert_shader.vert").c_str(),
		(shadersPath.string() + "/resources/shaders/mesh_dynfrag_shader.frag").c_str());

	// Delete all the triangles
	clear_triangles();

}


void dynamic_tri_list_store::add_tri(const int& tri_id, dynamic_line_store* edge1,
	dynamic_line_store* edge2,	dynamic_line_store* edge3)
{
	// Create a temporary triangle
	// Add to the list
	dynamic_tri_store* dyn_temp_tri = new dynamic_tri_store;

	dyn_temp_tri->tri_id = tri_id;
	dyn_temp_tri->edge1 = edge1;
	dyn_temp_tri->edge2 = edge2;
	dyn_temp_tri->edge3 = edge3;


	// Set the face normal


	//___________________________________________________________________
	// Add to the list
	dyn_triMap.push_back(dyn_temp_tri);

	// Add to the tri id map
	dyn_triId_Map.insert({ tri_id, dyn_tri_count });

	// Iterate the tri count
	dyn_tri_count++;
}

dynamic_tri_store* dynamic_tri_list_store::get_triangle(const int& dyn_tri_id)
{
	// Check whether tri_id exists?
	auto it = dyn_triId_Map.find(dyn_tri_id);

	if (it != dyn_triId_Map.end())
	{
		// tri id exists
		// return the address of the triangle
		return dyn_triMap[it->second];
	}
	else
	{
		// id not found
		return nullptr;
	}

}


void dynamic_tri_list_store::set_buffer()
{
	// Set the buffer for index
	unsigned int tri_indices_count = 3 * dyn_tri_count; // 3 indices to form a triangle
	unsigned int* tri_vertex_indices = new unsigned int[tri_indices_count];

	unsigned int tri_i_index = 0;

	// Set the node vertices
	for (auto& tri : dyn_triMap)
	{
		// Add tri buffers
		get_tri_index_buffer(tri_vertex_indices, tri_i_index);
	}

	VertexBufferLayout tri_pt_layout;
	tri_pt_layout.AddFloat(3);  // Node center
	tri_pt_layout.AddFloat(3);  // Node offset
	tri_pt_layout.AddFloat(1);  // Defl


	// Define the node vertices of the model for a node (3 position, 3 defl, 3 color, 3 Edge 025, 3 Edge 050, 3 Edge 075 ) 
	const unsigned int tri_vertex_count = 7 * 3 * dyn_tri_count;
	unsigned int tri_vertex_size = tri_vertex_count * sizeof(float); // Size of the node_vertex

	// Create the Node Deflection buffers
	dyn_tri_buffer.CreateDynamicBuffers(tri_vertex_size, tri_vertex_indices, tri_indices_count, tri_pt_layout);

	// Delete the dynamic array
	delete[] tri_vertex_indices;
}

void dynamic_tri_list_store::paint_triangles()
{
	// Paint all the triangles
	dyn_tri_shader.Bind();
	dyn_tri_buffer.Bind();

	// tri buffer updated seperately for dynamic drawing (To draw cyclic data ex. modal vectors)

	glDrawElements(GL_TRIANGLES, (3 * dyn_tri_count), GL_UNSIGNED_INT, 0);
	dyn_tri_buffer.UnBind();
	dyn_tri_shader.UnBind();

}

void dynamic_tri_list_store::update_buffer(const int& dyn_index)
{
	// Define the node vertices of the model for a node (3 position, 3 defl, 3 color, 3 Edge 025, 3 Edge 050, 3 Edge 075 ) 
	const unsigned int tri_vertex_count = 7 * 3 * dyn_tri_count;
	float* tri_vertices = new float[tri_vertex_count];

	unsigned int tri_v_index = 0;

	// Set the tri point vertices
	for (auto& tri : dyn_triMap)
	{
		// Add points buffers
		get_tri_vertex_buffer(tri, dyn_index, tri_vertices, tri_v_index);
	}

	unsigned int tri_vertex_size = tri_vertex_count * sizeof(float); // Size of the tri point vertex

	// Update the buffer
	dyn_tri_buffer.UpdateDynamicVertexBuffer(tri_vertices, tri_vertex_size);

	// Delete the dynamic array
	delete[] tri_vertices;
}


void dynamic_tri_list_store::clear_triangles()
{
	// Delete all the triangles
	dyn_tri_count = 0;
	dyn_triId_Map.clear();
	dyn_triMap.clear();

}


void dynamic_tri_list_store::update_opengl_uniforms(bool set_modelmatrix, bool set_pantranslation, bool set_rotatetranslation,
	bool set_zoomtranslation, bool set_transparency, bool set_deflscale)
{
	if (set_modelmatrix == true)
	{
		// set the model matrix
		dyn_tri_shader.setUniform("geom_scale", static_cast<float>(geom_param_ptr->geom_scale));
		dyn_tri_shader.setUniform("transparency", 0.6f);

		dyn_tri_shader.setUniform("modelMatrix", geom_param_ptr->modelMatrix, false);
	}

	if (set_pantranslation == true)
	{
		// set the pan translation
		dyn_tri_shader.setUniform("panTranslation", geom_param_ptr->panTranslation, false);
	}

	if (set_rotatetranslation == true)
	{
		// set the rotate translation
		dyn_tri_shader.setUniform("rotateTranslation", geom_param_ptr->rotateTranslation, false);
	}

	if (set_zoomtranslation == true)
	{
		// set the zoom translation
		dyn_tri_shader.setUniform("zoomscale", static_cast<float>(geom_param_ptr->zoom_scale));
	}

	if (set_transparency == true)
	{
		// set the alpha transparency
		dyn_tri_shader.setUniform("transparency", static_cast<float>(geom_param_ptr->geom_transparency));
	}

	if (set_deflscale == true)
	{
		// set the deflection scale
		dyn_tri_shader.setUniform("normalized_deflscale", static_cast<float>(geom_param_ptr->normalized_defl_scale));
		dyn_tri_shader.setUniform("deflscale", static_cast<float>(geom_param_ptr->defl_scale));
	}

}

void dynamic_tri_list_store::get_tri_vertex_buffer(dynamic_tri_store* tri, const int& dyn_index,
	float* dyn_tri_vertices, unsigned int& dyn_tri_v_index)
{
	// Get the node buffer for the shader
	// Tri Point 1
	// Point location
	dyn_tri_vertices[dyn_tri_v_index + 0] = tri->edge1->start_pt->point_loc.x;
	dyn_tri_vertices[dyn_tri_v_index + 1] = tri->edge1->start_pt->point_loc.y;
	dyn_tri_vertices[dyn_tri_v_index + 2] = tri->edge1->start_pt->point_loc.z;

	// Point offset
	dyn_tri_vertices[dyn_tri_v_index + 3] = tri->edge1->start_pt->point_offset[dyn_index].x;
	dyn_tri_vertices[dyn_tri_v_index + 4] = tri->edge1->start_pt->point_offset[dyn_index].y;
	dyn_tri_vertices[dyn_tri_v_index + 5] = tri->edge1->start_pt->point_offset[dyn_index].z;

	// Normalized deflection value
	dyn_tri_vertices[dyn_tri_v_index + 6] = tri->edge1->start_pt->point_offset_mag[dyn_index];

	// Iterate
	dyn_tri_v_index = dyn_tri_v_index + 7;

	// Tri Point 2
	// Point location
	dyn_tri_vertices[dyn_tri_v_index + 0] = tri->edge2->start_pt->point_loc.x;
	dyn_tri_vertices[dyn_tri_v_index + 1] = tri->edge2->start_pt->point_loc.y;
	dyn_tri_vertices[dyn_tri_v_index + 2] = tri->edge2->start_pt->point_loc.z;

	// Point offset
	dyn_tri_vertices[dyn_tri_v_index + 3] = tri->edge2->start_pt->point_offset[dyn_index].x;
	dyn_tri_vertices[dyn_tri_v_index + 4] = tri->edge2->start_pt->point_offset[dyn_index].y;
	dyn_tri_vertices[dyn_tri_v_index + 5] = tri->edge2->start_pt->point_offset[dyn_index].z;

	// Normalized deflection value
	dyn_tri_vertices[dyn_tri_v_index + 6] = tri->edge2->start_pt->point_offset_mag[dyn_index];
	
	// Iterate
	dyn_tri_v_index = dyn_tri_v_index + 7;


	// Tri Point 3
	// Point location
	dyn_tri_vertices[dyn_tri_v_index + 0] = tri->edge3->start_pt->point_loc.x;
	dyn_tri_vertices[dyn_tri_v_index + 1] = tri->edge3->start_pt->point_loc.y;
	dyn_tri_vertices[dyn_tri_v_index + 2] = tri->edge3->start_pt->point_loc.z;

	// Point offset
	dyn_tri_vertices[dyn_tri_v_index + 3] = tri->edge3->start_pt->point_offset[dyn_index].x;
	dyn_tri_vertices[dyn_tri_v_index + 4] = tri->edge3->start_pt->point_offset[dyn_index].y;
	dyn_tri_vertices[dyn_tri_v_index + 5] = tri->edge3->start_pt->point_offset[dyn_index].z;

	// Normalized deflection value
	dyn_tri_vertices[dyn_tri_v_index + 6] = tri->edge3->start_pt->point_offset_mag[dyn_index];
	
	// Iterate
	dyn_tri_v_index = dyn_tri_v_index + 7;
}


void dynamic_tri_list_store::get_tri_index_buffer(unsigned int* dyn_tri_vertex_indices, unsigned int& dyn_tri_i_index)
{
	// Add the indices
	// Index 1
	dyn_tri_vertex_indices[dyn_tri_i_index] = dyn_tri_i_index;

	dyn_tri_i_index = dyn_tri_i_index + 1;

	// Index 2
	dyn_tri_vertex_indices[dyn_tri_i_index] = dyn_tri_i_index;

	dyn_tri_i_index = dyn_tri_i_index + 1;

	// Index 3
	dyn_tri_vertex_indices[dyn_tri_i_index] = dyn_tri_i_index;

	dyn_tri_i_index = dyn_tri_i_index + 1;

}