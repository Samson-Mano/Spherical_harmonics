#include "pulse_elementline_list_store.h"

pulse_elementline_list_store::pulse_elementline_list_store()
{
	// Empty constructor
}

pulse_elementline_list_store::~pulse_elementline_list_store()
{
	// Empty destructor
}

void pulse_elementline_list_store::init(geom_parameters* geom_param_ptr)
{
	// Set the geometry parameters
	this->geom_param_ptr = geom_param_ptr;

	// Set the geometry parameters for the line
	pulse_element_lines.init(geom_param_ptr);

	max_line_displ = 0.0;

	// Clear the element lines
	pulse_elementline_count = 0;
	pulse_elementlineMap.clear();
}

void pulse_elementline_list_store::clear_data()
{
	// Dynamic lines
	pulse_element_lines.clear_lines();

	max_line_displ = 0.0;

	// Clear the element lines
	pulse_elementline_count = 0;
	pulse_elementlineMap.clear();
}

void pulse_elementline_list_store::add_pulse_elementline(int& line_id, pulse_node_store* startNode, pulse_node_store* endNode)
{
	// Add result line element
	pulse_elementline_store temp_pulse_line;
	temp_pulse_line.line_id = line_id;
	temp_pulse_line.startNode = startNode;
	temp_pulse_line.endNode = endNode;

	// Check whether the line_id is already there
	if (pulse_elementlineMap.find(line_id) != pulse_elementlineMap.end())
	{
		// Element ID already exist (do not add)
		return;
	}

	//__________________________ Get the node points
	glm::vec3 temp_color = glm::vec3(0);
	glm::vec3 start_node_pt = (*startNode).node_pt;
	glm::vec3 end_node_pt = (*endNode).node_pt;


	//__________________________ Add result lines Displacement results
	// Displacement magnitude list
	temp_pulse_line.startpt_displ_magnitude = startNode->node_pulse_result.node_displ_magnitude;
	temp_pulse_line.endpt_displ_magnitude = endNode->node_pulse_result.node_displ_magnitude;

	// Displacement vector list
	temp_pulse_line.startpt_displ = startNode->node_pulse_result.node_displ;
	temp_pulse_line.endpt_displ = endNode->node_pulse_result.node_displ;

	// Insert to the lines
	pulse_elementlineMap.insert({ line_id, temp_pulse_line });
	pulse_elementline_count++;
}

void pulse_elementline_list_store::set_buffer()
{
	// Clear the lines
	pulse_element_lines.clear_lines();

	//__________________________ Add the Dynamic lines
	for (auto& line_m : pulse_elementlineMap)
	{
		pulse_elementline_store  rline = line_m.second;

		// Scale the displacement with the max pulse displacement
		std::vector<glm::vec3> startnode_displ;
		std::vector<double> startnode_displ_magnitude;
		std::vector<glm::vec3> endnode_displ;
		std::vector<double> endnode_displ_magnitude;

		for (int i = 0; i < static_cast<int>(rline.startpt_displ.size()); i++)
		{
			// Start Node
			startnode_displ.push_back(rline.startpt_displ[i] / static_cast<float>(max_line_displ));
			startnode_displ_magnitude.push_back(rline.startpt_displ_magnitude[i]/ static_cast<float>(max_line_displ ));

			// End Node
			endnode_displ.push_back(rline.endpt_displ[i] / static_cast<float>(max_line_displ));
			endnode_displ_magnitude.push_back(rline.endpt_displ_magnitude[i] / static_cast<float>(max_line_displ));

		}


		// Add to the line list
		pulse_element_lines.add_line(rline.line_id,
			rline.startNode->node_pt,
			rline.endNode->node_pt,
			startnode_displ, endnode_displ,
			startnode_displ_magnitude, endnode_displ_magnitude);

	}

	// Set the buffer (Only the index buffer is set because its a dynamic paint)
	pulse_element_lines.set_buffer();
}

void pulse_elementline_list_store::paint_pulse_elementlines(const int& dyn_index)
{
	// Paint the lines
	pulse_element_lines.paint_lines(dyn_index);
}

void pulse_elementline_list_store::update_geometry_matrices(bool set_modelmatrix, bool set_pantranslation, bool set_rotatetranslation,
	bool set_zoomtranslation, bool set_transparency, bool set_deflscale)
{
	// Result line update geometry 
	pulse_element_lines.update_opengl_uniforms(set_modelmatrix, set_pantranslation, set_rotatetranslation,
		set_zoomtranslation, set_transparency, set_deflscale);
}
