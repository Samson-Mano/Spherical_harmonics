#pragma once
#include "rslt_elementline_list_store.h"
#include "../geometry_objects/dynamic_tri_list_store.h"


struct rslt_elementtri_store
{
	int tri_id = 0; // ID of the triangle element
	rslt_node_store* nd1 = nullptr; // node 1
	rslt_node_store* nd2 = nullptr; // node 2
	rslt_node_store* nd3 = nullptr; // node 3

};


class rslt_elementtri_list_store
{
public:
	unsigned int rslt_elementtri_count = 0;
	std::unordered_map<int, rslt_elementtri_store> rslt_elementtriMap; // Create an unordered_map to store Triangles with ID as key

	rslt_elementtri_list_store();
	~rslt_elementtri_list_store();
	void init(geom_parameters* geom_param_ptr, dcel_dynmesh_data* mesh_data);
	void add_rslt_elementtriangle(int& tri_id, rslt_node_store* nd1, rslt_node_store* nd2,
		rslt_node_store* nd3);

	void clear_results();

private:
	geom_parameters* geom_param_ptr = nullptr;
	dcel_dynmesh_data* mesh_data = nullptr;

};
