#pragma once
#include "../fe_objects/nodes_list_store.h"
#include "../geometry_objects/dcel_dynmesh_data.h"

struct rslt_node_store
{
	int node_id = 0;
	glm::vec3 node_pt = glm::vec3(0);

	// results (x, y, z)
	std::vector<glm::vec3> node_displ;
	std::vector<double> node_displ_magnitude;
};

class rslt_nodes_list_store
{
public:
	unsigned int node_count = 0;
	std::unordered_map<int, rslt_node_store> rslt_nodeMap; // Create an unordered_map to store nodes with ID as key
	double rslt_maxdispl = 0.0;

	rslt_nodes_list_store();
	~rslt_nodes_list_store();
	void init(geom_parameters* geom_param_ptr, dcel_dynmesh_data* mesh_data);
	void add_result_node(int& node_id, const glm::vec3& node_pt, const std::vector<glm::vec3>& node_displ,
		const std::vector<double>& node_displ_magnitude);

	void clear_results();

private:
	geom_parameters* geom_param_ptr = nullptr;
	dcel_dynmesh_data* mesh_data = nullptr;

};
