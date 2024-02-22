#include "modal_elementquad_list_store.h"

modal_elementquad_list_store::modal_elementquad_list_store()
{
	// Empty Constructor
}

modal_elementquad_list_store::~modal_elementquad_list_store()
{
	// Empty Destructor
}

void modal_elementquad_list_store::init(geom_parameters* geom_param_ptr, dcel_dynmesh_data* mesh_data)
{
	// Set the geometry parameters
	this->geom_param_ptr = geom_param_ptr;
	this->mesh_data = mesh_data;


	// Clear the element quads
	modal_elementquad_count = 0;
	modal_elementquadMap.clear();
}


void modal_elementquad_list_store::add_modal_elementquadrilateral(int& quad_id, modal_node_store* nd1,
	modal_node_store* nd2, modal_node_store* nd3, modal_node_store* nd4, 
	glm::vec3& midpt, std::vector<glm::vec3>& midpt_modal_displ, std::vector<double>& midpt_modal_displ_mag)
{
	// Add result quad element
	modal_elementquad_store temp_quad;
	temp_quad.quad_id = quad_id;
	temp_quad.nd1 = nd1;
	temp_quad.nd2 = nd2;
	temp_quad.nd3 = nd3;
	temp_quad.nd4 = nd4;

	// Add the point coordinate
	temp_quad.nd1pt = (*nd1).node_pt;
	temp_quad.nd2pt = (*nd2).node_pt;
	temp_quad.nd3pt = (*nd3).node_pt;
	temp_quad.nd4pt = (*nd4).node_pt;
	temp_quad.midpt = midpt;

	// Add the modal displacement
	temp_quad.nd1_modal_displ = (*nd1).node_modal_displ;
	temp_quad.nd2_modal_displ = (*nd2).node_modal_displ;
	temp_quad.nd3_modal_displ = (*nd3).node_modal_displ;
	temp_quad.nd4_modal_displ = (*nd4).node_modal_displ;
	temp_quad.midpt_modal_displ = midpt_modal_displ;

	// Add the modal displacement magnitude
	temp_quad.nd1_modal_displ_mag = (*nd1).node_modal_displ_magnitude;
	temp_quad.nd2_modal_displ_mag = (*nd2).node_modal_displ_magnitude;
	temp_quad.nd3_modal_displ_mag = (*nd3).node_modal_displ_magnitude;
	temp_quad.nd4_modal_displ_mag = (*nd4).node_modal_displ_magnitude;
	temp_quad.midpt_modal_displ_mag = midpt_modal_displ_mag;


	// Check whether the Quad_id is already there
	if (modal_elementquadMap.find(quad_id) != modal_elementquadMap.end())
	{
		// Element ID already exist (do not add)
		return;
	}

	// Insert to the Quads
	modal_elementquadMap.insert({ quad_id, temp_quad });
	modal_elementquad_count++;

}
