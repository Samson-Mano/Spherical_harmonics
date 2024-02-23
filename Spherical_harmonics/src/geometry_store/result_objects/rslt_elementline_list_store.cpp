#include "rslt_elementline_list_store.h"

rslt_elementline_list_store::rslt_elementline_list_store()
{
	// Empty constructor
}

rslt_elementline_list_store::~rslt_elementline_list_store()
{
	// Empty destructor
}

void rslt_elementline_list_store::init(geom_parameters* geom_param_ptr, dcel_dynmesh_data* mesh_data)
{
	// Set the geometry parameters
	this->geom_param_ptr = geom_param_ptr;
	this->mesh_data = mesh_data;

	// Clear the result element lines
	clear_results();

}

void rslt_elementline_list_store::add_rslt_elementline(int& line_id, rslt_node_store* startNode, rslt_node_store* endNode)
{
	// Add result line element
	rslt_elementline_store temp_line;
	temp_line.line_id = line_id;
	temp_line.startNode = startNode;
	temp_line.endNode = endNode;

	//// Check whether the line_id is already there
	//if (rslt_elementlineMap.find(line_id) != rslt_elementlineMap.end())
	//{
	//	// Element ID already exist (do not add)
	//	return;
	//}

	// Insert to the lines
	rslt_elementlineMap.insert({ line_id, temp_line });
	rslt_elementline_count++;

	// Add to the result geometry lines

}

void rslt_elementline_list_store::clear_results()
{
	// Clear the results
	rslt_elementline_count = 0;
	rslt_elementlineMap.clear();

}