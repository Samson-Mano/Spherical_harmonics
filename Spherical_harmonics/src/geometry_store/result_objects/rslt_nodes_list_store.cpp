#include "rslt_nodes_list_store.h"

rslt_nodes_list_store::rslt_nodes_list_store()
{
	// Empty constructor
}

rslt_nodes_list_store::~rslt_nodes_list_store()
{
	// Empty destructor
}

void rslt_nodes_list_store::init(geom_parameters* geom_param_ptr, dcel_dynmesh_data* mesh_data)
{
	// Set the geometry parameters
	this->geom_param_ptr = geom_param_ptr;
	this->mesh_data = mesh_data;

	// Clear the node results
	clear_results();

}

void rslt_nodes_list_store::add_result_node(int& node_id, const glm::vec3& node_pt, 
	const std::vector<glm::vec3>& node_displ, const std::vector<double>& node_displ_magnitude)
{
	// Add result nodes
	rslt_node_store temp_node;
	temp_node.node_id = node_id;
	temp_node.node_pt = node_pt;
	temp_node.node_displ = node_displ;
	temp_node.node_displ_magnitude = node_displ_magnitude;

	//// Check whether the node_id is already there
	//if (modal_nodeMap.find(node_id) != modal_nodeMap.end())
	//{
	//	// Node ID already exist (do not add)
	//	return;
	//}

	// Insert to the nodes
	rslt_nodeMap.insert({ node_id, temp_node });
	rslt_node_count++;

	//__________________________ Add the result node points
	mesh_data->add_mesh_point(node_id, node_pt, node_displ, node_displ_magnitude);

}

void rslt_nodes_list_store::set_max_displacement(const double& rslt_maxdispl)
{
	// set the node result maximum displacement
	this->rslt_maxdispl = rslt_maxdispl;

	// Set the same to point maximum displacement
	mesh_data->node_points.point_maxdispl = rslt_maxdispl;
}

void rslt_nodes_list_store::clear_results()
{
	// Clear the results
	rslt_node_count = 0;
	rslt_nodeMap.clear();
	rslt_maxdispl = 0.0;

}