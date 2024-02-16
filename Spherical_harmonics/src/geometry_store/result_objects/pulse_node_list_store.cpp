#include "pulse_node_list_store.h"

pulse_node_list_store::pulse_node_list_store()
{
	// Empty constructor
}

pulse_node_list_store::~pulse_node_list_store()
{
	// Empty destructor
}

void pulse_node_list_store::init(geom_parameters* geom_param_ptr)
{
	// Initialize
	// Set the geometry parameters
	this->geom_param_ptr = geom_param_ptr;

	// Set the geometry parameters for the points
	pulse_node_points.init(geom_param_ptr);

	// Clear the results
	node_count = 0;
	pulse_nodeMap.clear();
	pulse_node_points.clear_points();
	max_node_displ = 0.0; // Maximum nodal displacement
}

void pulse_node_list_store::clear_data()
{
	// Clear the results
	node_count = 0;
	pulse_nodeMap.clear();
	pulse_node_points.clear_points();
	max_node_displ = 0.0; // Maximum nodal displacement
}

void pulse_node_list_store::add_result_node(int& node_id, const glm::vec3& node_pt,
	const pulse_node_result& node_pulse_result,
	const int& number_of_timesteps)
{
	// Add the result nodes
	pulse_node_store temp_pulse_node;

	temp_pulse_node.node_id = node_id; // Add the node ID
	temp_pulse_node.node_pt = node_pt; // Add the node point
	temp_pulse_node.node_pulse_result = node_pulse_result; // node pulse result
	temp_pulse_node.number_of_timesteps = number_of_timesteps; // Number of time steps

	// Check whether the node_id is already there
	if (pulse_nodeMap.find(node_id) != pulse_nodeMap.end())
	{
		// Node ID already exist (do not add)
		return;
	}

	// Add to the pulse nodeMap
	pulse_nodeMap.insert({ node_id,temp_pulse_node });
	node_count++;
}

void pulse_node_list_store::set_buffer()
{
	// Clear the points
	pulse_node_points.clear_points();

	//__________________________ Add the Dynamic points
	for (auto& nd_m : pulse_nodeMap)
	{
		pulse_node_store nd = nd_m.second; // get the node data

		// Scale the displacement with the max pulse displacement
		std::vector<glm::vec3> node_displ;
		std::vector<double> node_displ_magnitude;

		for (int i = 0; i < static_cast<int>(nd.node_pulse_result.node_displ.size()); i++)
		{
			node_displ.push_back(nd.node_pulse_result.node_displ[i] / static_cast<float>(max_node_displ));
			node_displ_magnitude.push_back(nd.node_pulse_result.node_displ_magnitude[i]/ static_cast<float>(max_node_displ));
		}


		// Add to the pulse points
		pulse_node_points.add_point(nd.node_id, nd.node_pt, node_displ, node_displ_magnitude);
	}

	// Set buffer
	pulse_node_points.set_buffer();


}

void pulse_node_list_store::paint_pulse_nodes(const int& dyn_index)
{
	// Paint the points
	pulse_node_points.paint_points(dyn_index);
}

void pulse_node_list_store::update_geometry_matrices(bool set_modelmatrix, bool set_pantranslation, bool set_rotatetranslation, 
	bool set_zoomtranslation, bool set_transparency, bool set_deflscale)
{
	// Pulse node points update geometry 
	pulse_node_points.update_opengl_uniforms(set_modelmatrix, set_pantranslation, set_rotatetranslation, set_zoomtranslation, set_transparency, set_deflscale);
}
