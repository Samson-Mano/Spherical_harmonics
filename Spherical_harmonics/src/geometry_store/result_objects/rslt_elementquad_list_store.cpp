#include "rslt_elementquad_list_store.h"

rslt_elementquad_list_store::rslt_elementquad_list_store()
{
	// Empty constructor
}

rslt_elementquad_list_store::~rslt_elementquad_list_store()
{
	// Empty destructor
}

void rslt_elementquad_list_store::init(geom_parameters* geom_param_ptr, dcel_dynmesh_data* mesh_data)
{
	// Set the geometry parameters
	this->geom_param_ptr = geom_param_ptr;
	this->mesh_data = mesh_data;


	// Clear the element quads
	clear_results();

}

void rslt_elementquad_list_store::add_rslt_elementquadrilateral(int& quad_id, rslt_node_store* nd1, rslt_node_store* nd2, 
	rslt_node_store* nd3, rslt_node_store* nd4)
{
	// Add result quad element
	rslt_elementquad_store temp_quad;
	temp_quad.quad_id = quad_id;
	temp_quad.nd1 = nd1;
	temp_quad.nd2 = nd2;
	temp_quad.nd3 = nd3;
	temp_quad.nd4 = nd4;

	//// Check whether the Quad_id is already there
	//if (rslt_elementquadMap.find(quad_id) != rslt_elementquadMap.end())
	//{
	//	// Element ID already exist (do not add)
	//	return;
	//}

	// Insert to the Quads
	rslt_elementquadMap.insert({ quad_id, temp_quad });
	rslt_elementquad_count++;

	//__________________________ Add the result element quadrilaterals
	mesh_data->add_mesh_quadrilateral(quad_id, (*nd1).node_id, (*nd2).node_id, (*nd3).node_id, (*nd4).node_id);

}

void rslt_elementquad_list_store::clear_results()
{
	// Clear the results
	rslt_elementquad_count = 0;
	rslt_elementquadMap.clear();
}