#include "nodeload_list_store.h"

nodeload_list_store::nodeload_list_store()
{
	// Empty constructor
}

nodeload_list_store::~nodeload_list_store()
{
	// Empty destructor
}

void nodeload_list_store::init(geom_parameters* geom_param_ptr)
{
	// Set the geometry parameters
	this->geom_param_ptr = geom_param_ptr;

	load_value_labels.init(geom_param_ptr);

	// Create the shader and Texture for the drawing the constraints
	std::filesystem::path shadersPath = geom_param_ptr->resourcePath;

	load_shader.create_shader((shadersPath.string() + "/resources/shaders/load_vert_shader.vert").c_str(),
		(shadersPath.string() + "/resources/shaders/load_frag_shader.frag").c_str());

	// Clear the loads
	load_count = 0;
	load_max = 0.0;
	loadMap.clear();
	all_load_ids.clear();
}

void nodeload_list_store::set_zero_condition(const int& model_type)
{
	this->model_type = model_type; // Model type 0 - Circular, 1,2,3 Rectangular
}

void nodeload_list_store::add_loads(int& node_id, glm::vec3& load_loc, double& load_start_time,
	double& load_end_time, double& load_value)
{
	load_data temp_load;
	temp_load.load_id = get_unique_load_id(all_load_ids); // Load id
	temp_load.node_id = node_id; // id of the line its applied to
	temp_load.load_loc = load_loc; // Load location
	temp_load.load_start_time = load_start_time; // Load start time
	temp_load.load_end_time = load_end_time; // Load end time
	temp_load.load_value = load_value; // Load value

	// Insert the load to line
	loadMap.insert({ temp_load.load_id, temp_load });
	all_load_ids.push_back(temp_load.load_id); // Add to the id vector
	load_count++;
}

void nodeload_list_store::delete_load(int& node_id)
{
	if (load_count == 0)
	{
		return;
	}

	// Delete all the loads in the node
	std::vector<int> delete_load_id;

	for (auto& ldx : loadMap)
	{
		load_data ld = ldx.second;

		// Check whether the load's nodeID is the nodeID
		if (ld.node_id == node_id)
		{
			// Add to the delete load ID
			delete_load_id.push_back(ld.load_id);
		}
	}

	//____________________________________

	for (auto& del_id : delete_load_id)
	{
		// Delete load ID
		loadMap.erase(del_id);

		// Delete the iD from the load ids
		auto it = std::find(all_load_ids.begin(), all_load_ids.end(), del_id);
		all_load_ids.erase(it);


		// Reduce the load count
		load_count--;
	}

}

void nodeload_list_store::set_buffer()
{
	// Set the buffer for Loads
	if (load_count == 0)
	{
		// No load to paint
		// Set the load lables
		load_value_labels.clear_labels();
		return;
	}

	// Set the load max
	// Load Max
	load_max = 0.0;
	// Set the load lables
	load_value_labels.clear_labels();

	// Find the load maximum
	for (auto& loadx : loadMap)
	{
		load_data load = loadx.second;

		if (load_max < std::abs(load.load_value))
		{
			load_max = std::abs(load.load_value);
		}
		//__________________________________________________________________________

		if (load.show_load_label == true)
		{
			std::stringstream ss;
			ss << std::fixed << std::setprecision(geom_param_ptr->load_precision) << std::abs(load.load_value);

			glm::vec3 temp_color = geom_param_ptr->geom_colors.load_color;
			std::string	temp_str = "(" + std::to_string(load.node_id) + ") " + ss.str();
			double load_angle_rad = 0.0f;

			bool is_load_val_above = false;
			if (load.load_value < 0)
			{
				is_load_val_above = true;
			}

			load_value_labels.add_text(temp_str, load.load_loc, glm::vec3(0), temp_color, load_angle_rad, is_load_val_above, false);
		}
	}

	load_value_labels.set_buffer();

	//__________________________________________________________________________

	unsigned int load_vertex_count = 5 * 9 * load_count;
	float* load_vertices = new float[load_vertex_count];

	unsigned int load_indices_count = 14 * load_count;
	unsigned int* load_indices = new unsigned int[load_indices_count];

	unsigned int load_v_index = 0;
	unsigned int load_i_index = 0;

	for (auto& ldx : loadMap)
	{
		load_data ld = ldx.second;

		// Add the load buffer
		get_load_buffer(ld, load_vertices, load_v_index, load_indices, load_i_index);
	}

	VertexBufferLayout load_layout;
	load_layout.AddFloat(3);  // Position
	load_layout.AddFloat(3);  // Center
	load_layout.AddFloat(3);  // Color

	unsigned int load_vertex_size = load_vertex_count * sizeof(float);

	// Create the Constraint buffers
	load_buffer.CreateBuffers(load_vertices, load_vertex_size,
		load_indices, load_indices_count, load_layout);

	// Delete the Dynamic arrays
	delete[] load_vertices;
	delete[] load_indices;
}

void nodeload_list_store::paint_loads()
{
	// Paint the loads
	load_shader.Bind();
	load_buffer.Bind();
	glDrawElements(GL_LINES, 14 * load_count, GL_UNSIGNED_INT, 0);
	load_buffer.UnBind();
	load_shader.UnBind();
}

void nodeload_list_store::paint_load_labels()
{
	// Paint load labels
	load_value_labels.paint_text();
}

void nodeload_list_store::update_geometry_matrices(bool set_modelmatrix, bool set_pantranslation, bool set_rotatetranslation,
	bool set_zoomtranslation, bool set_transparency, bool set_deflscale)
{
	// Update the load value label uniforms
	load_value_labels.update_opengl_uniforms(set_modelmatrix, set_pantranslation, set_rotatetranslation, set_zoomtranslation, set_transparency, set_deflscale);

	if (set_modelmatrix == true)
	{
		// set the model matrix
		load_shader.setUniform("geom_scale", static_cast<float>(geom_param_ptr->geom_scale));
		load_shader.setUniform("transparency", 1.0f);


		load_shader.setUniform("projectionMatrix", geom_param_ptr->projectionMatrix, false);
		load_shader.setUniform("viewMatrix", geom_param_ptr->viewMatrix, false);
		load_shader.setUniform("modelMatrix", geom_param_ptr->modelMatrix, false);
	}

	if (set_pantranslation == true)
	{
		// set the pan translation
		load_shader.setUniform("panTranslation", geom_param_ptr->panTranslation, false);
	}

	if (set_rotatetranslation == true)
	{
		// set the rotate translation
		load_shader.setUniform("rotateTranslation", geom_param_ptr->rotateTranslation, false);
	}

	if (set_zoomtranslation == true)
	{
		// set the zoom translation
		load_shader.setUniform("zoomscale", static_cast<float>(geom_param_ptr->zoom_scale));
	}

	if (set_transparency == true)
	{
		// set the alpha transparency
		load_shader.setUniform("transparency", static_cast<float>(geom_param_ptr->geom_transparency));
	}

	if (set_deflscale == true)
	{
		// set the deflection scale
		// load_shader.setUniform("deflscale", static_cast<float>(geom_param_ptr->defl_scale));
	}
}

void nodeload_list_store::get_load_buffer(load_data& ld, float* load_vertices, unsigned int& load_v_index, unsigned int* load_indices, unsigned int& load_i_index)
{
	int load_sign = ld.load_value > 0 ? 1 : -1;

	glm::vec3 load_loc = ld.load_loc;
	glm::vec3 load_color = geom_param_ptr->geom_colors.load_color;

	// Rotate the corner points
	glm::vec3 load_arrow_startpt = glm::normalize(load_loc) *
		static_cast<float>(- 1.0f * load_sign * (geom_param_ptr->node_circle_radii / geom_param_ptr->geom_scale)); // 0
	glm::vec3 load_arrow_endpt = glm::normalize(load_loc) *
		static_cast<float>(- 20.0f * (ld.load_value / load_max) * (geom_param_ptr->node_circle_radii /geom_param_ptr->geom_scale)); // 1
	//___________________________

	glm::vec3 load_arrow_pt1 = glm::vec3(0, -2.0f * load_sign * (geom_param_ptr->node_circle_radii / static_cast<float>(geom_param_ptr->geom_scale)),
		-5.0f * load_sign * (geom_param_ptr->node_circle_radii / static_cast<float>(geom_param_ptr->geom_scale))); // 2
	glm::vec3 load_arrow_pt2 = glm::vec3(1.732f * load_sign * (geom_param_ptr->node_circle_radii / static_cast<float>(geom_param_ptr->geom_scale)),
		 1.0f * load_sign * (geom_param_ptr->node_circle_radii / static_cast<float>(geom_param_ptr->geom_scale)), 
		-5.0f * load_sign * (geom_param_ptr->node_circle_radii / static_cast<float>(geom_param_ptr->geom_scale))); // 3
	glm::vec3 load_arrow_pt3 = glm::vec3(-1.732f * load_sign * (geom_param_ptr->node_circle_radii / static_cast<float>(geom_param_ptr->geom_scale)),
		 1.0f * load_sign * (geom_param_ptr->node_circle_radii / static_cast<float>(geom_param_ptr->geom_scale)),
		-5.0f * load_sign * (geom_param_ptr->node_circle_radii / static_cast<float>(geom_param_ptr->geom_scale))); // 4

	//__________________________________________________________________________________________________________

	// Load 0th point
	// Position
	load_vertices[load_v_index + 0] = load_loc.x + load_arrow_startpt.x;
	load_vertices[load_v_index + 1] = load_loc.y + load_arrow_startpt.y;
	load_vertices[load_v_index + 2] = load_loc.z + load_arrow_startpt.z;

	// Load location center
	load_vertices[load_v_index + 3] = load_loc.x;
	load_vertices[load_v_index + 4] = load_loc.y;
	load_vertices[load_v_index + 5] = load_loc.z;

	// Load color
	load_vertices[load_v_index + 6] = load_color.x;
	load_vertices[load_v_index + 7] = load_color.y;
	load_vertices[load_v_index + 8] = load_color.z;

	load_v_index = load_v_index + 9;

	// Load 1th point
	// Position
	load_vertices[load_v_index + 0] = load_loc.x + load_arrow_endpt.x;
	load_vertices[load_v_index + 1] = load_loc.y + load_arrow_endpt.y;
	load_vertices[load_v_index + 2] = load_loc.z + load_arrow_endpt.z;

	// Load location center
	load_vertices[load_v_index + 3] = load_loc.x;
	load_vertices[load_v_index + 4] = load_loc.y;
	load_vertices[load_v_index + 5] = load_loc.z;

	// Load color
	load_vertices[load_v_index + 6] = load_color.x;
	load_vertices[load_v_index + 7] = load_color.y;
	load_vertices[load_v_index + 8] = load_color.z;

	load_v_index = load_v_index + 9;

	// Load 2th point
	// Position
	load_vertices[load_v_index + 0] = load_loc.x + load_arrow_pt1.x;
	load_vertices[load_v_index + 1] = load_loc.y + load_arrow_pt1.y;
	load_vertices[load_v_index + 2] = load_loc.z + load_arrow_pt1.z;

	// Load location center
	load_vertices[load_v_index + 3] = load_loc.x;
	load_vertices[load_v_index + 4] = load_loc.y;
	load_vertices[load_v_index + 5] = load_loc.z;

	// Load color
	load_vertices[load_v_index + 6] = load_color.x;
	load_vertices[load_v_index + 7] = load_color.y;
	load_vertices[load_v_index + 8] = load_color.z;

	load_v_index = load_v_index + 9;

	// Load 3th point
	// Position
	load_vertices[load_v_index + 0] = load_loc.x + load_arrow_pt2.x;
	load_vertices[load_v_index + 1] = load_loc.y + load_arrow_pt2.y;
	load_vertices[load_v_index + 2] = load_loc.z + load_arrow_pt2.z;

	// Load location center
	load_vertices[load_v_index + 3] = load_loc.x;
	load_vertices[load_v_index + 4] = load_loc.y;
	load_vertices[load_v_index + 5] = load_loc.z;

	// Load color
	load_vertices[load_v_index + 6] = load_color.x;
	load_vertices[load_v_index + 7] = load_color.y;
	load_vertices[load_v_index + 8] = load_color.z;

	load_v_index = load_v_index + 9;

	// Load 4th point
	// Position
	load_vertices[load_v_index + 0] = load_loc.x + load_arrow_pt3.x;
	load_vertices[load_v_index + 1] = load_loc.y + load_arrow_pt3.y;
	load_vertices[load_v_index + 2] = load_loc.z + load_arrow_pt3.z;

	// Load location center
	load_vertices[load_v_index + 3] = load_loc.x;
	load_vertices[load_v_index + 4] = load_loc.y;
	load_vertices[load_v_index + 5] = load_loc.z;

	// Load color
	load_vertices[load_v_index + 6] = load_color.x;
	load_vertices[load_v_index + 7] = load_color.y;
	load_vertices[load_v_index + 8] = load_color.z;

	load_v_index = load_v_index + 9;

	//______________________________________________________________________
	// 
	// Set the Load indices
	unsigned int t_id = ((load_i_index / 14) * 5);

	// Line 0,1
	load_indices[load_i_index + 0] = t_id + 0;
	load_indices[load_i_index + 1] = t_id + 1;

	// Line 0,2
	load_indices[load_i_index + 2] = t_id + 0;
	load_indices[load_i_index + 3] = t_id + 2;

	// Line 0,3
	load_indices[load_i_index + 4] = t_id + 0;
	load_indices[load_i_index + 5] = t_id + 3;

	// Line 0,4
	load_indices[load_i_index + 6] = t_id + 0;
	load_indices[load_i_index + 7] = t_id + 4;

	// Line 2,3
	load_indices[load_i_index + 8] = t_id + 2;
	load_indices[load_i_index + 9] = t_id + 3;

	// Line 3,4
	load_indices[load_i_index + 10] = t_id + 3;
	load_indices[load_i_index + 11] = t_id + 4;

	// Line 4,2
	load_indices[load_i_index + 12] = t_id + 4;
	load_indices[load_i_index + 13] = t_id + 2;

	// Increment
	load_i_index = load_i_index + 14;
}

int nodeload_list_store::get_unique_load_id(std::vector<int>& all_ids)
{
	// Return the unique Load id
	if (all_ids.size() != 0)
	{
		int i;
		std::sort(all_ids.begin(), all_ids.end());

		// Find if any of the nodes are missing in an ordered int
		for (i = 0; i < all_ids.size(); i++)
		{
			if (all_ids[i] != i)
			{
				return i;
			}
		}

		// no node id is missing in an ordered list so add to the end
		return static_cast<int>(all_ids.size());
	}
	return 0;
}

