#pragma once
#include "pulse_node_list_store.h"
#include "../geometry_objects/dynamic_line_list_store.h"


struct pulse_elementline_store
{
	int line_id = 0; // ID of the line
	pulse_node_store* startNode = nullptr; // start node
	pulse_node_store* endNode = nullptr; // end node

	// Line result displacement data
	// Line displacement magnitude
	std::vector<double> startpt_displ_magnitude; // Pt1 displacmenet magnitude at time t
	std::vector<double> endpt_displ_magnitude; // Pt2 displacmenet magnitude at time t

	// Line normalized displacement vector
	std::vector<glm::vec3> startpt_displ;
	std::vector<glm::vec3> endpt_displ;

};


class pulse_elementline_list_store
{
public:
	unsigned int pulse_elementline_count = 0;
	std::unordered_map<int, pulse_elementline_store> pulse_elementlineMap; // Create an unordered_map to store lines with ID as key
	double max_line_displ = 0.0; // Maximum line displacement

	pulse_elementline_list_store();
	~pulse_elementline_list_store();
	void init(geom_parameters* geom_param_ptr);
	void clear_data();
	void add_pulse_elementline(int& line_id, pulse_node_store* startNode, pulse_node_store* endNode);
	void set_buffer();
	void paint_pulse_elementlines(const int& dyn_index);
	void update_geometry_matrices(bool set_modelmatrix, bool set_pantranslation, bool set_rotatetranslation,
		bool set_zoomtranslation, bool set_transparency, bool set_deflscale);

private:
	geom_parameters* geom_param_ptr = nullptr;
	dynamic_line_list_store pulse_element_lines;

};
