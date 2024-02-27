#pragma once
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <unordered_map>
#include "../geom_parameters.h"
#include "../geometry_buffers/gBuffers.h"
#include "../geometry_objects/dcel_mesh_data.h"

struct node_store
{
	int node_id = -1;
	double x_coord = 0.0;
	double y_coord = 0.0;
	double z_coord = 0.0;

	glm::vec3 node_pt() const 
	{
		return glm::vec3(x_coord, y_coord, z_coord);
	}
};


class nodes_list_store
{
public:
	unsigned int node_count = 0;
	std::unordered_map<int, node_store> nodeMap; // Create an unordered_map to store nodes with ID as key


	nodes_list_store();
	~nodes_list_store();
	void init(geom_parameters* geom_param_ptr, dcel_mesh_data* mesh_data);
	void add_node(const int& node_id,  const double& x_coord, const double& y_coord, const double& z_coord);

	std::vector<int> is_node_selected(const glm::vec2& corner_pt1, const glm::vec2& corner_pt2);

private:
	geom_parameters* geom_param_ptr = nullptr;
	dcel_mesh_data* mesh_data = nullptr;

};
