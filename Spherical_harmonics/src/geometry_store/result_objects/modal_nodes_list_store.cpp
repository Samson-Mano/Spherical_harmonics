#include "modal_nodes_list_store.h"

modal_nodes_list_store::modal_nodes_list_store()
{
	// Empty constructor
}

modal_nodes_list_store::~modal_nodes_list_store()
{
	// Empty destructor
}

void modal_nodes_list_store::init(geom_parameters* geom_param_ptr, dcel_dynmesh_data* mesh_data)
{
	// Set the geometry parameters
	this->geom_param_ptr = geom_param_ptr;
	this->mesh_data = mesh_data;

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

	//// Check whether the node_id is already there
	//if (modal_nodeMap.find(node_id) != modal_nodeMap.end())
	//{
	//	// Node ID already exist (do not add)
	//	return;
	//}

	// Insert to the nodes
	modal_nodeMap.insert({ node_id, temp_node });
	node_count++;

	//__________________________ Add the result node points
	mesh_data->add_mesh_point(node_id, node_pt,node_modal_displ,node_modal_displ_magnitude);

}
