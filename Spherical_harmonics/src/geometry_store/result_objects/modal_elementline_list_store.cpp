#include "modal_elementline_list_store.h"

modal_elementline_list_store::modal_elementline_list_store()
{
	// Empty constructor
}

modal_elementline_list_store::~modal_elementline_list_store()
{
	// Empty destructor
}

void modal_elementline_list_store::init(geom_parameters* geom_param_ptr, dcel_dynmesh_data* mesh_data)
{
	// Set the geometry parameters
	this->geom_param_ptr = geom_param_ptr;
	this->mesh_data = mesh_data;

	// Clear the element lines
	modal_elementline_count = 0;
	modal_elementlineMap.clear();
}


void modal_elementline_list_store::add_modal_elementline(int& line_id, modal_node_store* startNode, modal_node_store* endNode)
{
	// Add result line element
	modal_elementline_store temp_line;
	temp_line.line_id = line_id;
	temp_line.startNode = startNode;
	temp_line.endNode = endNode;

	// Add the point coordinate
	temp_line.startpt = (*startNode).node_pt;
	temp_line.endpt = (*endNode).node_pt;

	// Add the modal displacement
	temp_line.startnd_modal_displ = (*startNode).node_modal_displ;
	temp_line.endnd_modal_displ = (*endNode).node_modal_displ;

	// Add the modal displacement magnitude
	temp_line.startnd_modal_displ_mag = (*startNode).node_modal_displ_magnitude;
	temp_line.endnd_modal_displ_mag = (*endNode).node_modal_displ_magnitude;

	// Check whether the line_id is already there
	if (modal_elementlineMap.find(line_id) != modal_elementlineMap.end())
	{
		// Element ID already exist (do not add)
		return;
	}

	// Insert to the lines
	modal_elementlineMap.insert({ line_id, temp_line });
	modal_elementline_count++;
}

