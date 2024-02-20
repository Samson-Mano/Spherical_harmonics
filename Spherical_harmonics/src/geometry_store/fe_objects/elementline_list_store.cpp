#include "elementline_list_store.h"

elementline_list_store::elementline_list_store()
{
	// Empty constructor
}

elementline_list_store::~elementline_list_store()
{
	// Empty destructor
}

void elementline_list_store::init(geom_parameters* geom_param_ptr, dcel_mesh_data* mesh_data)
{
	// Set the geometry parameters
	this->geom_param_ptr = geom_param_ptr;
	this->mesh_data = mesh_data;

	// Clear the lines
	elementline_count = 0;
	elementlineMap.clear();
}

void elementline_list_store::add_elementline(const int& line_id, node_store* startNode, node_store* endNode)
{
	// Add the line to the list
	elementline_store temp_line;
	temp_line.line_id = line_id;
	temp_line.startNode = startNode;
	temp_line.endNode = endNode;

	//// Check whether the line_id is already there
	//if (elementlineMap.find(line_id) != elementlineMap.end())
	//{
	//	// Element ID already exist (do not add)
	//	return;
	//}
	
	// Insert to the lines
	elementlineMap.insert({ line_id, temp_line });
	elementline_count++;

}
