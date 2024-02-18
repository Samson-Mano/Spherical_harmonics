#include "line_list_store.h"

line_list_store::line_list_store()
{
	// Empty constructor
}

line_list_store::~line_list_store()
{
	// Empty destructor
}

void line_list_store::init(geom_parameters* geom_param_ptr)
{
	// Set the geometry parameters
	this->geom_param_ptr = geom_param_ptr;

	// Create the point shader
	std::filesystem::path shadersPath = geom_param_ptr->resourcePath;

	line_shader.create_shader((shadersPath.string() + "/resources/shaders/mesh_vert_shader.vert").c_str(),
		(shadersPath.string() + "/resources/shaders/mesh_frag_shader.frag").c_str());

	line_shader.setUniform("ptColor", geom_param_ptr->geom_colors.edge_color);

	// Delete all the labels
	clear_lines();
}

void line_list_store::add_line(const int& line_id, point_store* start_pt, point_store* end_pt)
{
	// Create a temporary points
	line_store temp_ln;
	temp_ln.line_id = line_id;

	// Line points
	temp_ln.start_pt = start_pt;
	temp_ln.end_pt = end_pt;

	// Add to the list
	lineMap.push_back(temp_ln);

	// Add to the line id map
	lineId_Map.insert({ line_id, line_count });

	// Iterate the point count
	line_count++;
}

line_store* line_list_store::get_line(const int& line_id)
{
	// Check whether line_id exists?
	auto it = lineId_Map.find(line_id);
	line_store* ln = nullptr;

	if (it != lineId_Map.end())
	{
		// line id exists
		ln = &lineMap[it->second];
	}
	else
	{
		// id not found
		return nullptr;
	}

	// return the address of the line
	return ln;
}


void line_list_store::set_buffer()
{
	// Define the node vertices of the model for a node (3 position) 
	const unsigned int line_vertex_count = 3 * 2 * line_count;
	float* line_vertices = new float[line_vertex_count];

	unsigned int line_indices_count = 2 * line_count; // 1 indices to form a point
	unsigned int* line_vertex_indices = new unsigned int[line_indices_count];

	unsigned int line_v_index = 0;
	unsigned int line_i_index = 0;

	// Set the node vertices
	for (auto& ln : lineMap)
	{
		// Add  points buffers
		get_line_buffer(ln, line_vertices, line_v_index, line_vertex_indices, line_i_index);
	}

	VertexBufferLayout line_pt_layout;
	line_pt_layout.AddFloat(3);  // Node center

	unsigned int line_vertex_size = line_vertex_count * sizeof(float); // Size of the node_vertex

	// Create the Node Deflection buffers
	line_buffer.CreateBuffers(line_vertices, line_vertex_size, line_vertex_indices, line_indices_count, line_pt_layout);

	// Set the point size
	// glPointSize(4.1f);

	// Delete the dynamic array
	delete[] line_vertices;
	delete[] line_vertex_indices;
}

void line_list_store::paint_lines()
{
	// Paint all the points
	line_shader.Bind();
	line_buffer.Bind();
	glDrawElements(GL_LINES, (2*line_count), GL_UNSIGNED_INT, 0);
	line_buffer.UnBind();
	line_shader.UnBind();
}

void line_list_store::clear_lines()
{
	// Delete all the lines
	line_count = 0;
	lineMap.clear();
	lineId_Map.clear();
}

void line_list_store::update_opengl_uniforms(bool set_modelmatrix, bool set_pantranslation, bool set_rotatetranslation,
	bool set_zoomtranslation, bool set_transparency)
{
	if (set_modelmatrix == true)
	{
		// set the model matrix
		line_shader.setUniform("geom_scale", static_cast<float>(geom_param_ptr->geom_scale));
		line_shader.setUniform("transparency", 1.0f);

		line_shader.setUniform("projectionMatrix", geom_param_ptr->projectionMatrix, false);
		line_shader.setUniform("viewMatrix", geom_param_ptr->viewMatrix, false);
		line_shader.setUniform("modelMatrix", geom_param_ptr->modelMatrix, false);
	}

	if (set_pantranslation == true)
	{
		// set the pan translation
		line_shader.setUniform("panTranslation", geom_param_ptr->panTranslation, false);
	}

	if (set_rotatetranslation == true)
	{
		// set the rotate translation
		line_shader.setUniform("rotateTranslation", geom_param_ptr->rotateTranslation, false);
	}

		if (set_zoomtranslation == true)
	{
		// set the zoom translation
		line_shader.setUniform("zoomscale", static_cast<float>(geom_param_ptr->zoom_scale));
	}

	if (set_transparency == true)
	{
		// set the alpha transparency
		line_shader.setUniform("transparency", static_cast<float>(geom_param_ptr->geom_transparency));
	}
}

void line_list_store::get_line_buffer(line_store& ln, float* line_vertices, unsigned int& line_v_index, unsigned int* line_vertex_indices, unsigned int& line_i_index)
{
	// Get the node buffer for the shader
	// Start Point
	// Point location
	line_vertices[line_v_index + 0] = ln.start_pt->x_coord;
	line_vertices[line_v_index + 1] = ln.start_pt->y_coord;
	line_vertices[line_v_index + 2] = ln.start_pt->z_coord;

	// Iterate
	line_v_index = line_v_index + 3;

	// End Point
	// Point location
	line_vertices[line_v_index + 0] = ln.end_pt->x_coord;
	line_vertices[line_v_index + 1] = ln.end_pt->y_coord;
	line_vertices[line_v_index + 2] = ln.end_pt->z_coord;

	// Iterate
	line_v_index = line_v_index + 3;

	//__________________________________________________________________________
	// Add the indices
	// Index 1
	line_vertex_indices[line_i_index] = line_i_index;

	line_i_index = line_i_index + 1;

	// Index 2
	line_vertex_indices[line_i_index] = line_i_index;

	line_i_index = line_i_index + 1;

}

