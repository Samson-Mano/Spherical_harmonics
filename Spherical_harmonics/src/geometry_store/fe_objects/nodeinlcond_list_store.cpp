#include "nodeinlcond_list_store.h"

nodeinlcond_list_store::nodeinlcond_list_store()
{
	// Empty constructor
}

nodeinlcond_list_store::~nodeinlcond_list_store()
{
	// Empty destructor
}

void nodeinlcond_list_store::init(geom_parameters* geom_param_ptr)
{
	// Set the geometry parameters
	this->geom_param_ptr = geom_param_ptr;

	// Set the geometry parameter for the points
	inlcond_points.init(geom_param_ptr);
	inlcond_lines.init(geom_param_ptr);

	inlcondMap.clear();
	inlcond_count = 0;
}

void nodeinlcond_list_store::set_zero_condition(int inlcond_type, const int& model_type)
{
	this->inlcond_type = inlcond_type; // Initial condition type 0 - Displacement, 1 - Velocity
	this->model_type = model_type; // Model type 0

	if (inlcond_type == 0)
	{
		// Displacement initial condition
		inlcond_points.update_point_color(geom_param_ptr->geom_colors.inlcond_displ_color);
		inlcond_lines.update_line_color(geom_param_ptr->geom_colors.inlcond_displ_color);

	}
	else
	{
		// Velocity initial condition
		inlcond_points.update_point_color(geom_param_ptr->geom_colors.inlcond_velo_color);
		inlcond_lines.update_line_color(geom_param_ptr->geom_colors.inlcond_velo_color);

	}

}

void nodeinlcond_list_store::add_inlcondition(int& node_id, glm::vec3& inlcond_loc, double& inl_amplitude_z)
{
	// Add the initial condition to the particular node
	nodeinl_condition_data temp_inl_condition_data;
	temp_inl_condition_data.node_id = node_id;
	temp_inl_condition_data.inlcond_loc = inlcond_loc;
	temp_inl_condition_data.inl_amplitude_z = inl_amplitude_z;

	// Insert the inital condition data to unordered map
	// Searching for node_id
	if (inlcondMap.find(node_id) != inlcondMap.end())
	{
		// Node is already have constraint
		// so remove the constraint
		inlcondMap[node_id] = temp_inl_condition_data;

		return;
	}

	// Insert the constraint to nodes
	inlcondMap.insert({ node_id, temp_inl_condition_data });
	inlcond_count++;

}


void nodeinlcond_list_store::delete_inlcondition(int& node_id)
{
	// Delete the initial condition in this node
	if (inlcond_count != 0)
	{
		// Remove the intial condition data to unordered map
		// Searching for node_id
		// Check there is already a initial conditon in the found node
		if (inlcondMap.find(node_id) != inlcondMap.end())
		{
			// Node is already have initial condition
			// so remove the intial condition
			inlcondMap.erase(node_id);

			// Update the buffer
			set_buffer();

			// adjust the initial condition count
			inlcond_count--;
		}
	}
}

void nodeinlcond_list_store::set_buffer()
{

	// Reset the points based on the addition of new inl condition points
	inlcond_points.clear_points();
	inlcond_lines.clear_lines();

	// Set the initial condition amplitude max
	// initial condition amplitude Max
	inlcond_max = 0.0;

	// Find the initial condition amplitude maximum
	for (auto& inlcond_m : inlcondMap)
	{
		nodeinl_condition_data inlcond = inlcond_m.second;

		if (inlcond_max < std::abs(inlcond.inl_amplitude_z))
		{
			inlcond_max = std::abs(inlcond.inl_amplitude_z);
		}
	}

	//if (inlcond_max == 0)
	//{
	//	// No initial condition value found
	//	return;
	//}

	// get the color
	glm::vec3 temp_color = glm::vec3(0);

	if (inlcond_type == 0)
	{
		// Initial Displacement
		temp_color = geom_param_ptr->geom_colors.inlcond_displ_color;
	}
	else if (inlcond_type == 1)
	{
		// Initial Velocity
		temp_color = geom_param_ptr->geom_colors.inlcond_velo_color;
	}

	//_________________________________________________________
	// Create the points
	int pt_id = 0;
	int ln_id = 0;

	for (auto& inlcond_m : inlcondMap)
	{
		nodeinl_condition_data inlcond = inlcond_m.second;

		// get the sign of initial condition amplitude
		// int inlcond_sign = inlcond.inl_amplitude_z > 0 ? 1 : -1;

		// initial condition point amplitude
		double pt_amplitude = 10.0f * (inlcond.inl_amplitude_z / inlcond_max) * (geom_param_ptr->node_circle_radii / static_cast<float>(geom_param_ptr->geom_scale));

		// initial condition point
		// glm::vec3 inlcond_pt_start = glm::vec3(inlcond.inlcond_loc.x, inlcond.inlcond_loc.y, inlcond.inlcond_loc.z);
		glm::vec3 inlcond_pt_end = glm::normalize(inlcond.inlcond_loc) * static_cast<float>(pt_amplitude);

		// Add the end point
		inlcond_points.add_point(pt_id, inlcond.inlcond_loc.x, inlcond.inlcond_loc.y, inlcond.inlcond_loc.z);

		pt_id++;

		// Add the end point
		inlcond_points.add_point(pt_id, inlcond.inlcond_loc.x + inlcond_pt_end.x,
			inlcond.inlcond_loc.y + inlcond_pt_end.y, inlcond.inlcond_loc.z + inlcond_pt_end.z);

		pt_id++;

		//// Add the initial condition line
		//glm::vec3 normal = glm::normalize(inlcond.inlcond_loc);
		//inlcond_lines.add_line(ln_id, inlcond_points.get_point(pt_id - 2), inlcond_points.get_point(pt_id - 1),normal);

		//ln_id++;
	}

	// Add the lines
	ln_id = 0;

	for (int i = 0; i < static_cast<int>(pt_id / 2.0f); i++)
	{
		// Add the initial condition line
		point_store* pt1 = inlcond_points.get_point((i * 2) + 0);
		point_store* pt2 = inlcond_points.get_point((i * 2) + 1);

		glm::vec3 normal = glm::normalize(pt1->pt_coord());

		inlcond_lines.add_line(ln_id, pt1, pt2, normal);

		ln_id++;
	}



	inlcond_points.set_buffer();
	inlcond_lines.set_buffer();

}

void nodeinlcond_list_store::paint_inlcond()
{
	// Paint the initial displacement points
	inlcond_points.paint_points();
	inlcond_lines.paint_lines();

}

void nodeinlcond_list_store::paint_inlcond_label()
{
	// Paint the peak displacement label
	// inl_condition_labels.paint_text();
}

void nodeinlcond_list_store::update_geometry_matrices(bool set_modelmatrix, bool set_pantranslation, bool set_rotatetranslation,
	bool set_zoomtranslation, bool set_transparency, bool set_deflscale)
{
	// Update model openGL uniforms
	inlcond_points.update_opengl_uniforms(set_modelmatrix, set_pantranslation, set_rotatetranslation, set_zoomtranslation, set_transparency);
	inlcond_lines.update_opengl_uniforms(set_modelmatrix, set_pantranslation, set_rotatetranslation, set_zoomtranslation, set_transparency);

}
