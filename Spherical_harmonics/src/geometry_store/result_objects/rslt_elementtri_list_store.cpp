#include "rslt_elementtri_list_store.h"

rslt_elementtri_list_store::rslt_elementtri_list_store()
{
	// Empty constructor
}

rslt_elementtri_list_store::~rslt_elementtri_list_store()
{
	// Empty destructor
}

void rslt_elementtri_list_store::init(geom_parameters* geom_param_ptr, dcel_dynmesh_data* mesh_data)
{
	// Set the geometry parameters
	this->geom_param_ptr = geom_param_ptr;
	this->mesh_data = mesh_data;

	// Clear the element tris
	clear_results();
}

void rslt_elementtri_list_store::add_rslt_elementtriangle(int& tri_id, rslt_node_store* nd1, rslt_node_store* nd2, rslt_node_store* nd3)
{
	// Add result tri element
	rslt_elementtri_store temp_tri;
	temp_tri.tri_id = tri_id;
	temp_tri.nd1 = nd1;
	temp_tri.nd2 = nd2;
	temp_tri.nd3 = nd3;


	//// Check whether the tri_id is already there
	//if (rslt_elementtriMap.find(tri_id) != rslt_elementtriMap.end())
	//{
	//	// Element ID already exist (do not add)
	//	return;
	//}

	// Insert to the tris
	rslt_elementtriMap.insert({ tri_id, temp_tri });
	rslt_elementtri_count++;

}

void rslt_elementtri_list_store::clear_results()
{
	// Clear the results
	rslt_elementtri_count = 0;
	rslt_elementtriMap.clear();

}