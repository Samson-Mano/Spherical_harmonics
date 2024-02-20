#include "elementtri_list_store.h"

elementtri_list_store::elementtri_list_store()
{
	// Empty constructor
}

elementtri_list_store::~elementtri_list_store()
{
	// Empty destructor
}

void elementtri_list_store::init(geom_parameters* geom_param_ptr, dcel_mesh_data* mesh_data)
{
	// Set the geometry parameters
	this->geom_param_ptr = geom_param_ptr;
	this->mesh_data = mesh_data;

	// Clear the triangles
	elementtri_count = 0;
	elementtriMap.clear();
}

void elementtri_list_store::add_elementtriangle(int& tri_id, node_store* nd1, node_store* nd2, node_store* nd3)
{
	// Add the Triangle to the list
	elementtri_store temp_tri;
	temp_tri.tri_id = tri_id; // Triangle ID
	temp_tri.nd1 = nd1;
	temp_tri.nd2 = nd2;
	temp_tri.nd3 = nd3;

	//// Check whether the node_id is already there
	//if (elementtriMap.find(tri_id) != elementtriMap.end())
	//{
	//	// Element ID already exist (do not add)
	//	return;
	//}

	// Insert to the triangles
	elementtriMap.insert({ tri_id, temp_tri });
	elementtri_count++;

	//__________________________ Add the Triangle
	mesh_data->add_mesh_triangle(tri_id, nd1->node_id, nd2->node_id, nd3->node_id);

}

