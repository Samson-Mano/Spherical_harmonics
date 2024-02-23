#pragma once
#include "rslt_elementline_list_store.h"
#include "../geometry_objects/dynamic_tri_list_store.h"


struct rslt_elementquad_store
{
	int quad_id = 0; // ID of the quadrilateral element
	rslt_node_store* nd1 = nullptr; // node 1
	rslt_node_store* nd2 = nullptr; // node 2
	rslt_node_store* nd3 = nullptr; // node 3
	rslt_node_store* nd4 = nullptr; // node 4


};


class rslt_elementquad_list_store
{
public:
	unsigned int rslt_elementquad_count = 0;
	std::unordered_map<int, rslt_elementquad_store> rslt_elementquadMap; // Create an unordered_map to store Quadrilaterals with ID as key

	rslt_elementquad_list_store();
	~rslt_elementquad_list_store();
	void init(geom_parameters* geom_param_ptr, dcel_dynmesh_data* mesh_data);
	void add_rslt_elementquadrilateral(int& quad_id, rslt_node_store* nd1, rslt_node_store* nd2,
		rslt_node_store* nd3, rslt_node_store* nd4);

	void clear_results();

private:
	geom_parameters* geom_param_ptr = nullptr;
	dcel_dynmesh_data* mesh_data = nullptr;

};
