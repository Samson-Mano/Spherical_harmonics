#include "point_list_store.h"

point_list_store::point_list_store()
{
	// Empty constructor
}

point_list_store::~point_list_store()
{
	// Empty destructor
}

void point_list_store::init(geom_parameters* geom_param_ptr)
{
	// Set the geometry parameters
	this->geom_param_ptr = geom_param_ptr;

	// Create the point shader
	std::filesystem::path shadersPath = geom_param_ptr->resourcePath;

	point_shader.create_shader((shadersPath.string() + "/resources/shaders/mesh_vert_shader.vert").c_str(),
		(shadersPath.string() + "/resources/shaders/mesh_frag_shader.frag").c_str());

	point_shader.setUniform("ptColor", geom_param_ptr->geom_colors.point_color);

	// Delete all the labels
	clear_points();
}

void point_list_store::add_point(const int& point_id, const double& x_coord, const double& y_coord, const double& z_coord)
{
	// Create a temporary points
	point_store temp_pt;
	temp_pt.point_id = point_id;
	temp_pt.x_coord = x_coord;
	temp_pt.y_coord = y_coord;
	temp_pt.z_coord = z_coord;

	// Add to the list
	pointMap.push_back(temp_pt);

	// Add to the point id map
	pointId_Map.insert({ point_id, point_count });

	// Iterate the point count
	point_count++;
}

point_store* point_list_store::get_point(const int& point_id)
{
	// Check whether point_id exists?
	auto it = pointId_Map.find(point_id);
	point_store* pt = nullptr;

	if (it != pointId_Map.end())
	{
		// Point id exists
		pt = &pointMap[it->second];
	}
	else
	{
		// id not found
		return nullptr;
	}

	// return the address of the point
	return pt;
}


void point_list_store::set_buffer()
{
	// Define the node vertices of the model for a node (3 position) 
	const unsigned int point_vertex_count = 3 * point_count;
	float* point_vertices = new float[point_vertex_count];

	unsigned int point_indices_count = 1 * point_count; // 1 indices to form a point
	unsigned int* point_vertex_indices = new unsigned int[point_indices_count];

	unsigned int point_v_index = 0;
	unsigned int point_i_index = 0;

	// Set the node vertices
	for (auto& pt : pointMap)
	{
		// Add points buffers
		get_node_buffer(pt, point_vertices, point_v_index, point_vertex_indices, point_i_index);
	}

	VertexBufferLayout node_layout;
	node_layout.AddFloat(3);  // Node center

	unsigned int point_vertex_size = point_vertex_count * sizeof(float); // Size of the node_vertex

	// Create the Node Deflection buffers
	point_buffer.CreateBuffers(point_vertices, point_vertex_size, point_vertex_indices, point_indices_count, node_layout);

	// Delete the dynamic array
	delete[] point_vertices;
	delete[] point_vertex_indices;
}

void point_list_store::paint_points()
{
	// Paint all the points
	point_shader.Bind();
	point_buffer.Bind();
	glDrawElements(GL_POINTS, point_count, GL_UNSIGNED_INT, 0);
	point_buffer.UnBind();
	point_shader.UnBind();
}

void point_list_store::clear_points()
{
	// Delete all the points
	point_count = 0;
	pointId_Map.clear();
	pointMap.clear();
}

void point_list_store::update_opengl_uniforms(bool set_modelmatrix, bool set_pantranslation, bool set_rotatetranslation,
	bool set_zoomtranslation, bool set_transparency)
{
	if (set_modelmatrix == true)
	{
		// set the model matrix
		point_shader.setUniform("geom_scale", static_cast<float>(geom_param_ptr->geom_scale));
		point_shader.setUniform("transparency", 1.0f);

		point_shader.setUniform("projectionMatrix", geom_param_ptr->projectionMatrix, false);
		point_shader.setUniform("viewMatrix", geom_param_ptr->viewMatrix, false);
		point_shader.setUniform("modelMatrix", geom_param_ptr->modelMatrix, false);
	}

	if (set_pantranslation == true)
	{
		// set the pan translation
		point_shader.setUniform("panTranslation", geom_param_ptr->panTranslation, false);
	}

	if (set_rotatetranslation == true)
	{
		// set the rotate translation
		point_shader.setUniform("rotateTranslation", geom_param_ptr->rotateTranslation, false);
	}

	if (set_zoomtranslation == true)
	{
		// set the zoom translation
		point_shader.setUniform("zoomscale", static_cast<float>(geom_param_ptr->zoom_scale));
	}

	if (set_transparency == true)
	{
		// set the alpha transparency
		point_shader.setUniform("transparency", static_cast<float>(geom_param_ptr->geom_transparency));
	}
}

void point_list_store::get_node_buffer(point_store& pt, float* point_vertices, unsigned int& point_v_index, unsigned int* point_indices, unsigned int& point_i_index)
{
	// Get the node buffer for the shader
	// Point location
	point_vertices[point_v_index + 0] = pt.x_coord;
	point_vertices[point_v_index + 1] = pt.y_coord;
	point_vertices[point_v_index + 2] = pt.z_coord;

	// Iterate
	point_v_index = point_v_index + 3;

	//__________________________________________________________________________
	// Add the indices
	point_indices[point_i_index] = point_i_index;

	point_i_index = point_i_index + 1;
}


