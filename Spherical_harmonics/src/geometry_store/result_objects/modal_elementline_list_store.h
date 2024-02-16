#pragma once
#include "modal_nodes_list_store.h"
#include "../geometry_objects/dynamic_line_list_store.h"


struct modal_elementline_store
{
	int line_id = 0; // ID of the line
	modal_node_store* startNode = nullptr; // start node
	modal_node_store* endNode = nullptr; // end node

	// Point coordinate
	glm::vec3 startpt = glm::vec3(0);
	glm::vec3 endpt = glm::vec3(0);

	// Point displacements
	std::vector<glm::vec3> startnd_modal_displ;
	std::vector<glm::vec3> endnd_modal_displ;

	// Point displacements magnitude
	std::vector<double> startnd_modal_displ_mag;
	std::vector<double> endnd_modal_displ_mag;
};

class modal_elementline_list_store
{
public:
	const int colormap_type = 1;
	unsigned int modal_elementline_count = 0;
	std::unordered_map<int, modal_elementline_store> modal_elementlineMap; // Create an unordered_map to store lines with ID as key
	
	modal_elementline_list_store();
	~modal_elementline_list_store();
	void init(geom_parameters* geom_param_ptr);
	void clear_data();
	void add_modal_elementline(int& line_id, modal_node_store* startNode, modal_node_store* endNode);
	void update_buffer(int selected_mode);
	void set_buffer();
	void paint_modal_elementlines();
	void update_geometry_matrices(bool set_modelmatrix, bool set_pantranslation, bool set_rotatetranslation,
		bool set_zoomtranslation, bool set_transparency, bool set_deflscale);
private:
	geom_parameters* geom_param_ptr = nullptr;
	dynamic_line_list_store modal_element_lines;

};