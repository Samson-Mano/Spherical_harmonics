#pragma once
#include "nodes_list_store.h"
#include "../geometry_objects/line_list_store.h"

struct elementline_store
{
	int line_id = 0; // ID of the line
	node_store* startNode = nullptr; // start node
	node_store* endNode = nullptr; // end node
};

class elementline_list_store
{
public:
	unsigned int elementline_count = 0;
	std::unordered_map<int, elementline_store> elementlineMap; // Create an unordered_map to store nodes with ID as key

	elementline_list_store();
	~elementline_list_store();
	void init(geom_parameters* geom_param_ptr, dcel_mesh_data* mesh_data);
	void add_elementline(const int& line_id, node_store* startNode, node_store* endNode);

private:
	geom_parameters* geom_param_ptr = nullptr;
	dcel_mesh_data* mesh_data = nullptr;

};
