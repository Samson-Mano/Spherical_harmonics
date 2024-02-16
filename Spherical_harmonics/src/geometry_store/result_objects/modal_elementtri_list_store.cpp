#include "modal_elementtri_list_store.h"

modal_elementtri_list_store::modal_elementtri_list_store()
{
	// Empty constructor
}

modal_elementtri_list_store::~modal_elementtri_list_store()
{
	// Empty destructor
}

void modal_elementtri_list_store::init(geom_parameters* geom_param_ptr)
{
	// Set the geometry parameters
	this->geom_param_ptr = geom_param_ptr;

	// Set the geometry parameters for the tris (and clear the tris)
	modal_element_tris.init(geom_param_ptr); // Tri 1

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

void modal_elementtri_list_store::clear_data()
{
	// Clear the element tris
	modal_element_tris.clear_triangles(); // Tri

	// Clear the element tris
	modal_elementtri_count = 0;
	modal_elementtriMap.clear();

}

void modal_elementtri_list_store::set_buffer()
{
	// Clear existing modal tris (8 triangles)
	modal_element_tris.clear_triangles(); // Tri 1m2

	// Add the tris
	// Loop throug every tri element
	int i = 0;
	for (auto& tri_m : modal_elementtriMap)
	{
		modal_elementtri_store tri = tri_m.second;

		// Add to the 1 triangle list
		modal_element_tris.add_tri(i, tri.nd1pt, tri.nd2pt, tri.nd3pt,
			tri.nd1_modal_displ, tri.nd2_modal_displ, tri.nd3_modal_displ,
			tri.nd1_modal_displ_mag, tri.nd2_modal_displ_mag, tri.nd3_modal_displ_mag); // Tri


		i++;
	}

	// Set the buffer
	modal_element_tris.set_buffer(); // Tri

}

void modal_elementtri_list_store::update_buffer(int selected_mode)
{
	// Update the buffer based on the selected mode
	modal_element_tris.update_buffer(selected_mode); // Tri 

}

void modal_elementtri_list_store::paint_modal_elementtriangles()
{
	// Paint the triangles
	modal_element_tris.paint_triangles(); // Tri 

}

void modal_elementtri_list_store::update_geometry_matrices(bool set_modelmatrix, bool set_pantranslation, bool set_rotatetranslation, bool set_zoomtranslation, bool set_transparency, bool set_deflscale)
{
	// Result tris update geometry 
	modal_element_tris.update_opengl_uniforms(set_modelmatrix, set_pantranslation, set_rotatetranslation,
		set_zoomtranslation, set_transparency, set_deflscale);

}
