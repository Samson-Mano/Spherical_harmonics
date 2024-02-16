#pragma once
#include "pulse_elementline_list_store.h"
#include "../geometry_objects/dynamic_tri_list_store.h"


struct pulse_elementtri_store
{
	int tri_id = 0; // ID of the triangle element
	pulse_node_store* nd1 = nullptr; // node 1
	pulse_node_store* nd2 = nullptr; // node 2
	pulse_node_store* nd3 = nullptr; // node 3

	// Point coordinate
	glm::vec3 nd1pt = glm::vec3(0);
	glm::vec3 nd2pt = glm::vec3(0);
	glm::vec3 nd3pt = glm::vec3(0);

	// Point displacements
	std::vector<glm::vec3> nd1_displ;
	std::vector<glm::vec3> nd2_displ;
	std::vector<glm::vec3> nd3_displ;

	// Point displacement magnitude
	std::vector<double> nd1_displ_mag;
	std::vector<double> nd2_displ_mag;
	std::vector<double> nd3_displ_mag;

};

class pulse_elementtri_list_store
{
public:
	unsigned int pulse_elementtri_count = 0;
	std::unordered_map<int, pulse_elementtri_store> pulse_elementtriMap; // Create an unordered_map to store tris with ID as key
	double max_tri_displ = 0.0; // Maximum tri displacement

	pulse_elementtri_list_store();
	~pulse_elementtri_list_store();
	void init(geom_parameters* geom_param_ptr);
	void clear_data();
	void add_pulse_elementtriangle(int& tri_id, pulse_node_store* nd1, pulse_node_store* nd2,
		pulse_node_store* nd3);
	void set_buffer();
	void paint_pulse_elementtriangles(const int& dyn_index);
	void update_geometry_matrices(bool set_modelmatrix, bool set_pantranslation, bool set_rotatetranslation,
		bool set_zoomtranslation, bool set_transparency, bool set_deflscale);

private:
	geom_parameters* geom_param_ptr = nullptr;
	// Anti clockwise
		//   2____3
		//   |	 /
		//   |	/
		//   | /			     
		//   1  

	dynamic_tri_list_store pulse_element_tris; // Tri

};
