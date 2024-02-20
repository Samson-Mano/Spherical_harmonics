#include "elementquad_list_store.h"

elementquad_list_store::elementquad_list_store()
{
	// Empty Constructor
}

elementquad_list_store::~elementquad_list_store()
{
	// Empty Destructor
}

void elementquad_list_store::init(geom_parameters* geom_param_ptr, dcel_mesh_data* mesh_data)
{
	// Set the geometry parameters
	this->geom_param_ptr = geom_param_ptr;
	this->mesh_data = mesh_data;

	// Clear the triangles
	elementquad_count = 0;
	elementquadMap.clear();
}

void elementquad_list_store::add_elementquadrilateral(int& quad_id, node_store* nd1, node_store* nd2, node_store* nd3, node_store* nd4)
{
	// Add the quadrilateral to the list
	elementquad_store temp_quad;
	temp_quad.quad_id = quad_id; // Quadrilateral ID
	temp_quad.nd1 = nd1;
	temp_quad.nd2 = nd2;
	temp_quad.nd3 = nd3;
	temp_quad.nd4 = nd4;

	//// Check whether the node_id is already there
	//if (elementquadMap.find(quad_id) != elementquadMap.end())
	//{
	//	// Element ID already exist (do not add)
	//	return;
	//}

	// Insert to the quadrilaterals
	elementquadMap.insert({ quad_id, temp_quad });
	elementquad_count++;

	//__________________________ Add the Quadrilateral
	mesh_data->add_mesh_quadrilateral(quad_id, nd1->node_id, nd2->node_id, nd3->node_id, nd4->node_id);

}

