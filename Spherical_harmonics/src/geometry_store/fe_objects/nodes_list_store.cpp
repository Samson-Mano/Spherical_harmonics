#include "nodes_list_store.h"

nodes_list_store::nodes_list_store()
{
	// Empty constructor
}

nodes_list_store::~nodes_list_store()
{
	// Empty destructor
}

void nodes_list_store::init(geom_parameters* geom_param_ptr,dcel_mesh_data* mesh_data)
{
	// Set the geometry parameters
	this->geom_param_ptr = geom_param_ptr;
	this->mesh_data = mesh_data;

	// Clear the nodes
	node_count = 0;
	nodeMap.clear();

}

void nodes_list_store::add_node(const int& node_id, const double& x_coord, const double& y_coord, const double& z_coord)
{
	// Add the node to the list
	node_store temp_node;
	temp_node.node_id = node_id;
	temp_node.x_coord = x_coord; // x coordinate
	temp_node.y_coord = y_coord; // y coordinate
	temp_node.z_coord = z_coord; // z coordinate

	//// Check whether the node_id is already there
	//if (nodeMap.find(node_id) != nodeMap.end())
	//{
	//	// Node ID already exist (do not add)
	//	return;
	//}

	// Insert to the nodes
	nodeMap.insert({ node_id, temp_node });
	node_count++;

	//__________________________ Add the node points
	mesh_data->add_mesh_point(node_id, x_coord, y_coord, z_coord);

}


std::vector<int> nodes_list_store::is_node_selected(const glm::vec2& corner_pt1, const glm::vec2& corner_pt2)
{
	// Return the node id of node which is inside the rectangle
	// Covert mouse location to screen location
	int max_dim = geom_param_ptr->window_width > geom_param_ptr->window_height ? geom_param_ptr->window_width : geom_param_ptr->window_height;

	// Selected node list index;
	std::vector<int> selected_node_index;

	// Transform the mouse location to openGL screen coordinates
	// Corner Point 1
	glm::vec2 screen_cpt1 = glm::vec2(2.0f * ((corner_pt1.x - (geom_param_ptr->window_width * 0.5f)) / max_dim),
		2.0f * (((geom_param_ptr->window_height * 0.5f) - corner_pt1.y) / max_dim));

	// Corner Point 2
	glm::vec2 screen_cpt2 = glm::vec2(2.0f * ((corner_pt2.x - (geom_param_ptr->window_width * 0.5f)) / max_dim),
		2.0f * (((geom_param_ptr->window_height * 0.5f) - corner_pt2.y) / max_dim));

	// Nodal location
	glm::mat4 scaling_matrix = glm::mat4(1.0) * static_cast<float>(geom_param_ptr->zoom_scale);
	scaling_matrix[3][3] = 1.0f;

	glm::mat4 scaledModelMatrix =geom_param_ptr->rotateTranslation *  scaling_matrix * geom_param_ptr->modelMatrix;


	// Loop through all nodes in map
	for (auto it = nodeMap.begin(); it != nodeMap.end(); it++)
	{
		node_store node = it->second;
		// const auto& node = it->second.node_pt;
		glm::vec4 finalPosition = scaledModelMatrix * glm::vec4(node.x_coord, node.y_coord, node.z_coord, 1.0f) * geom_param_ptr->panTranslation;

		glm::vec3 FragPos = glm::vec3(finalPosition.x, finalPosition.y, finalPosition.z);

		// Dot product check to only select the visible nodes !!!
		float v_dot = glm::dot(glm::vec3(0.0f,0.0f,1.0f), glm::normalize(FragPos));

		// Check whether the point inside a rectangle
		if (geom_param_ptr->isPointInsideRectangle(screen_cpt1, screen_cpt2, finalPosition) == true && v_dot > 0.0)
		{
			selected_node_index.push_back(it->first);
		}
	}

	// Return the node index find
	return selected_node_index;
}

