#pragma once
#include "elementline_list_store.h"
#include "../geometry_objects/tri_list_store.h"


struct elementtri_store
{
	int tri_id = 0; // ID of the triangle element
	node_store* nd1 = nullptr; // node 1
	node_store* nd2 = nullptr; // node 2
	node_store* nd3 = nullptr; // node 3
};

class elementtri_list_store
{
public:
	unsigned int elementtri_count = 0;
	std::unordered_map<int, elementtri_store> elementtriMap; // Create an unordered_map to store Triangles with ID as key

	elementtri_list_store();
	~elementtri_list_store();

	void init(geom_parameters* geom_param_ptr, dcel_mesh_data* mesh_data);
	void add_elementtriangle(int& tri_id, node_store* nd1, node_store* nd2, node_store* nd3);

private:
	geom_parameters* geom_param_ptr = nullptr;
	dcel_mesh_data* mesh_data = nullptr;

};
