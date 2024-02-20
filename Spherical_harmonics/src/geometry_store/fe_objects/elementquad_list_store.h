#pragma once
#include "elementline_list_store.h"
#include "../geometry_objects/tri_list_store.h"

struct elementquad_store
{
	int quad_id = 0; // ID of the quadrilateral element
	node_store* nd1 = nullptr; // node 1
	node_store* nd2 = nullptr; // node 2
	node_store* nd3 = nullptr; // node 3
	node_store* nd4 = nullptr; // node 4
};

class elementquad_list_store
{
public:
	unsigned int elementquad_count = 0;
	std::unordered_map<int, elementquad_store> elementquadMap; // Create an unordered_map to store Quadrilaterals with ID as key

	elementquad_list_store();
	~elementquad_list_store();
	void init(geom_parameters* geom_param_ptr, dcel_mesh_data* mesh_data);
	void add_elementquadrilateral(int& quad_id, node_store* nd1, node_store* nd2, node_store* nd3, node_store* nd4);

private:
	geom_parameters* geom_param_ptr = nullptr;
	dcel_mesh_data* mesh_data = nullptr;

};
