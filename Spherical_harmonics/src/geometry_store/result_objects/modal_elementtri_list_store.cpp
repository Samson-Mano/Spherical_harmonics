#include "modal_elementtri_list_store.h"

modal_elementtri_list_store::modal_elementtri_list_store()
{
	// Empty constructor
}

modal_elementtri_list_store::~modal_elementtri_list_store()
{
	// Empty destructor
}

void modal_elementtri_list_store::init(geom_parameters* geom_param_ptr, dcel_dynmesh_data* mesh_data)
{
	// Set the geometry parameters
	this->geom_param_ptr = geom_param_ptr;
	this->mesh_data = mesh_data;

	// Clear the element tris
	modal_elementtri_count = 0;
	modal_elementtriMap.clear();
}

void modal_elementtri_list_store::add_modal_elementtriangle(int& tri_id, modal_node_store* nd1, modal_node_store* nd2, modal_node_store* nd3)
{
	// Add result tri element
	modal_elementtri_store temp_tri;
	temp_tri.tri_id = tri_id;
	temp_tri.nd1 = nd1;
	temp_tri.nd2 = nd2;
	temp_tri.nd3 = nd3;

	// Add the point coordinate
	temp_tri.nd1pt = (*nd1).node_pt;
	temp_tri.nd2pt = (*nd2).node_pt;
	temp_tri.nd3pt = (*nd3).node_pt;

	// Add the modal displacement
	temp_tri.nd1_modal_displ = (*nd1).node_modal_displ;
	temp_tri.nd2_modal_displ = (*nd2).node_modal_displ;
	temp_tri.nd3_modal_displ = (*nd3).node_modal_displ;

	// Add the modal displacement magnitude
	temp_tri.nd1_modal_displ_mag = (*nd1).node_modal_displ_magnitude;
	temp_tri.nd2_modal_displ_mag = (*nd2).node_modal_displ_magnitude;
	temp_tri.nd3_modal_displ_mag = (*nd3).node_modal_displ_magnitude;


	// Check whether the tri_id is already there
	if (modal_elementtriMap.find(tri_id) != modal_elementtriMap.end())
	{
		// Element ID already exist (do not add)
		return;
	}

	// Insert to the tris
	modal_elementtriMap.insert({ tri_id, temp_tri });
	modal_elementtri_count++;

}
