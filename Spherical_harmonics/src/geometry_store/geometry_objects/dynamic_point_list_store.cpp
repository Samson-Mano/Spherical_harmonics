#include "dynamic_point_list_store.h"

dynamic_point_list_store::dynamic_point_list_store()
{
	// Empty constructor
}

dynamic_point_list_store::~dynamic_point_list_store()
{
	// Empty destructor
}

void dynamic_point_list_store::init(geom_parameters* geom_param_ptr)
{
	// Set the geometry parameters
	this->geom_param_ptr = geom_param_ptr;

	// Create the point shader
	std::filesystem::path shadersPath = geom_param_ptr->resourcePath;

	dyn_point_shader.create_shader((shadersPath.string() + "/resources/shaders/mesh_dynvert_shader.vert").c_str(),
		(shadersPath.string() + "/resources/shaders/mesh_dynfrag_shader.frag").c_str());

	// Delete all the points
	clear_points();
}

void dynamic_point_list_store::add_point(const int& point_id, const glm::vec3& point_loc, const std::vector<glm::vec3>& point_offset, 
	const std::vector<double>& point_offset_mag)
{
	dynamic_point_store dyn_temp_pt;
	dyn_temp_pt.point_id = point_id;
	dyn_temp_pt.point_loc = point_loc;
	dyn_temp_pt.point_normal = glm::normalize(point_loc); // Point normal
	dyn_temp_pt.point_offset = point_offset; // Dynamic point offset
	dyn_temp_pt.point_offset_mag = point_offset_mag; // Dynamic point magnitude

	//___________________________________________________________________
	// Add to the list
	dyn_pointMap.push_back(dyn_temp_pt);

	// Add to the point id map
	dyn_pointId_Map.insert({ point_id, dyn_point_count });

	// Iterate the point count
	dyn_point_count++;
}


dynamic_point_store* dynamic_point_list_store::get_point(const int& point_id)
{
	// Check whether point_id exists?
	auto it = dyn_pointId_Map.find(point_id);

	if (it != dyn_pointId_Map.end())
	{
		// Point id exists
		// return the address of the point
		return &dyn_pointMap[it->second];
	}
	else
	{
		// id not found
		return nullptr;
	}
}

void dynamic_point_list_store::set_buffer()
{
	// Set the buffer for index
	unsigned int point_indices_count = 1 * dyn_point_count; // 1 indices to form a point
	unsigned int* point_vertex_indices = new unsigned int[point_indices_count];

	unsigned int point_i_index = 0;

	// Set the node vertices
	for (auto& pt : dyn_pointMap)
	{
		// Add point index buffers
		get_point_index_buffer(point_vertex_indices, point_i_index);
	}

	VertexBufferLayout node_layout;
	node_layout.AddFloat(3);  // Node center
	node_layout.AddFloat(3);  // Node normal
	node_layout.AddFloat(3);  // Node offset
	node_layout.AddFloat(1);  // Node noramlized offset values

	// Define the node vertices of the model for a node (3 position, 3 normal, 3 defl, 1 normalized defl value) 
	const unsigned int point_vertex_count = 10 * dyn_point_count;
	unsigned int point_vertex_size = point_vertex_count * sizeof(float); // Size of the node_vertex

	// Allocate space for Vertex buffer
	// Create the Node Deflection buffers
	dyn_point_buffer.CreateDynamicBuffers(point_vertex_size, point_vertex_indices, point_indices_count, node_layout);

	// Delete the dynamic array
	delete[] point_vertex_indices;
}


void dynamic_point_list_store::paint_points()
{
	// Paint all the points
	dyn_point_shader.Bind();
	dyn_point_buffer.Bind();

	// point buffer updated seperately for dynamic drawing (To draw cyclic data ex. modal vectors)

	glDrawElements(GL_POINTS, dyn_point_count, GL_UNSIGNED_INT, 0);

	dyn_point_buffer.UnBind();
	dyn_point_shader.UnBind();
}


void dynamic_point_list_store::update_buffer(const int& dyn_index)
{
	// Define the node vertices of the model for a node (3 position, 3 normal, 3 defl, 3 color  & 1 defl value) 
	const unsigned int point_vertex_count = 10 * dyn_point_count;
	float* point_vertices = new float[point_vertex_count];

	unsigned int point_v_index = 0;

	// Set the node vertices
	for (auto& pt : dyn_pointMap)
	{
		// Add points buffers
		get_point_vertex_buffer(pt, dyn_index, point_vertices, point_v_index);
	}

	unsigned int point_vertex_size = point_vertex_count * sizeof(float); // Size of the node_vertex

	// Update the buffer
	dyn_point_buffer.UpdateDynamicVertexBuffer(point_vertices, point_vertex_size);

	// Delete the dynamic array
	delete[] point_vertices;
}


void dynamic_point_list_store::clear_points()
{
	// Delete all the points
	dyn_point_count = 0;
	dyn_pointId_Map.clear();
	dyn_pointMap.clear();
}

void dynamic_point_list_store::update_opengl_uniforms(bool set_modelmatrix,	bool set_pantranslation, bool set_rotatetranslation,
	bool set_zoomtranslation, bool set_transparency, bool set_deflscale)
{
	if (set_modelmatrix == true)
	{
		// set the model matrix
		dyn_point_shader.setUniform("geom_scale", static_cast<float>(geom_param_ptr->geom_scale));
		dyn_point_shader.setUniform("transparency", 1.0f);

		dyn_point_shader.setUniform("modelMatrix", geom_param_ptr->modelMatrix, false);
	}

	if (set_pantranslation == true)
	{
		// set the pan translation
		dyn_point_shader.setUniform("panTranslation", geom_param_ptr->panTranslation, false);
	}

	if (set_rotatetranslation == true)
	{
		// set the rotate translation
		dyn_point_shader.setUniform("rotateTranslation", geom_param_ptr->rotateTranslation, false);
	}

	if (set_zoomtranslation == true)
	{
		// set the zoom translation
		dyn_point_shader.setUniform("zoomscale", static_cast<float>(geom_param_ptr->zoom_scale));
	}

	if (set_transparency == true)
	{
		// set the alpha transparency
		// dyn_point_shader.setUniform("transparency", static_cast<float>(geom_param_ptr->geom_transparency));
	}

	if (set_deflscale == true)
	{
		// set the deflection scale
		dyn_point_shader.setUniform("normalized_deflscale", static_cast<float>(geom_param_ptr->normalized_defl_scale));
		dyn_point_shader.setUniform("deflscale", static_cast<float>(geom_param_ptr->defl_scale));
	}
}


void dynamic_point_list_store::renormalize_displmagnitude(const double& max_displacement)
{
	// Re-Normalize the maximum displacement

	for (auto& pt : dyn_pointMap)
	{
		for (int i = 0; i <static_cast<int>( pt.point_offset.size()); i++)
		{
			pt.point_offset[i] /= max_displacement;
			pt.point_offset_mag[i] /= max_displacement;
		}
	}

}


void dynamic_point_list_store::get_point_vertex_buffer(dynamic_point_store& pt,const int& dyn_index,
	float* dyn_point_vertices, unsigned int& dyn_point_v_index)
{
	// Get the node buffer for the shader
	// Point location
	dyn_point_vertices[dyn_point_v_index + 0] = pt.point_loc.x;
	dyn_point_vertices[dyn_point_v_index + 1] = pt.point_loc.y;
	dyn_point_vertices[dyn_point_v_index + 2] = pt.point_loc.z;

	// Point normal
	dyn_point_vertices[dyn_point_v_index + 3] = pt.point_normal.x;
	dyn_point_vertices[dyn_point_v_index + 4] = pt.point_normal.y;
	dyn_point_vertices[dyn_point_v_index + 5] = pt.point_normal.z;

	// Point offset
	dyn_point_vertices[dyn_point_v_index + 6] = pt.point_offset[dyn_index].x;
	dyn_point_vertices[dyn_point_v_index + 7] = pt.point_offset[dyn_index].y;
	dyn_point_vertices[dyn_point_v_index + 8] = pt.point_offset[dyn_index].z;

	// Point color
	dyn_point_vertices[dyn_point_v_index + 9] = pt.point_offset_mag[dyn_index];

	// Iterate
	dyn_point_v_index = dyn_point_v_index + 10;
}

void dynamic_point_list_store::get_point_index_buffer(unsigned int* dyn_point_indices, unsigned int& dyn_point_i_index)
{
	//__________________________________________________________________________
	// Add the indices
	dyn_point_indices[dyn_point_i_index] = dyn_point_i_index;

	dyn_point_i_index = dyn_point_i_index + 1;
}