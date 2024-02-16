#include "modal_nodes_list_store.h"

modal_nodes_list_store::modal_nodes_list_store()
{
	// Empty constructor
}

modal_nodes_list_store::~modal_nodes_list_store()
{
	// Empty destructor
}

void modal_nodes_list_store::init(geom_parameters* geom_param_ptr)
{
	// Set the geometry parameters
	this->geom_param_ptr = geom_param_ptr;

	// Set the geometry parameters for the labels (and clear the labels)
	modal_node_points.init(geom_param_ptr);

	// Clear the results
	node_count = 0;
	modal_nodeMap.clear();
}

void modal_nodes_list_store::clear_data()
{
	modal_node_points.clear_points();

	// Clear the results
	node_count = 0;
	modal_nodeMap.clear();
}

void modal_nodes_list_store::add_result_node(int& node_id, const glm::vec3& node_pt,const std::vector<glm::vec3>& node_modal_displ,
	const std::vector<double>& node_modal_displ_magnitude)
{
	// Add result nodes
	modal_node_store temp_node;
	temp_node.node_id = node_id;
	temp_node.node_pt = node_pt;
	temp_node.node_modal_displ = node_modal_displ;
	temp_node.node_modal_displ_magnitude = node_modal_displ_magnitude;

	// Check whether the node_id is already there
	if (modal_nodeMap.find(node_id) != modal_nodeMap.end())
	{
		// Node ID already exist (do not add)
		return;
	}

	// Insert to the nodes
	modal_nodeMap.insert({ node_id, temp_node });
	node_count++;
}

void modal_nodes_list_store::set_buffer()
{
	// Clear existing modal line
	modal_node_points.clear_points();

	// Add the dynamic points
	// Loop through every point
	int i = 0;
	for (auto& nd_m : modal_nodeMap)
	{
		modal_node_store nd = nd_m.second;

		// Add all the points
		modal_node_points.add_point(i, nd.node_pt, nd.node_modal_displ,nd.node_modal_displ_magnitude);

		i++;
	}

	// Set the buffer
	modal_node_points.set_buffer();
}


void modal_nodes_list_store::update_buffer(const int& selected_mode)
{
	modal_node_points.update_buffer(selected_mode);
}


void modal_nodes_list_store::paint_modal_nodes()
{
	modal_node_points.paint_points();
}


void modal_nodes_list_store::update_geometry_matrices(bool set_modelmatrix, bool set_pantranslation, bool set_rotatetranslation, 
	bool set_zoomtranslation, bool set_transparency, bool set_deflscale)
{
	modal_node_points.update_opengl_uniforms(set_modelmatrix, set_pantranslation, set_rotatetranslation, set_zoomtranslation, set_transparency, set_deflscale);
}
