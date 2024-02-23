#pragma once
#include "rslt_nodes_list_store.h"
#include "../geometry_objects/dcel_dynmesh_data.h"


struct rslt_elementline_store
{
	int line_id = 0; // ID of the line
	rslt_node_store* startNode = nullptr; // start node
	rslt_node_store* endNode = nullptr; // end node
};

class rslt_elementline_list_store
{
public:
	unsigned int rslt_elementline_count = 0;
	std::unordered_map<int, rslt_elementline_store> rslt_elementlineMap; // Create an unordered_map to store lines with ID as key

	rslt_elementline_list_store();
	~rslt_elementline_list_store();
	void init(geom_parameters* geom_param_ptr, dcel_dynmesh_data* mesh_data);
	void add_rslt_elementline(int& line_id, rslt_node_store* startNode, rslt_node_store* endNode);

	void clear_results();

private:
	geom_parameters* geom_param_ptr = nullptr;
	dcel_dynmesh_data* mesh_data = nullptr;

};