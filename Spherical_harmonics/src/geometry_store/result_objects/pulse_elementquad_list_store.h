#pragma once
#include "pulse_elementline_list_store.h"
#include "../geometry_objects/dynamic_tri_list_store.h"


struct pulse_elementquad_store
{
	int quad_id = 0; // ID of the quadrilateral element
	pulse_node_store* nd1 = nullptr; // node 1
	pulse_node_store* nd2 = nullptr; // node 2
	pulse_node_store* nd3 = nullptr; // node 3
	pulse_node_store* nd4 = nullptr; // node 4

	// Point coordinate
	glm::vec3 nd1pt = glm::vec3(0);
	glm::vec3 nd2pt = glm::vec3(0);
	glm::vec3 nd3pt = glm::vec3(0);
	glm::vec3 nd4pt = glm::vec3(0);
	glm::vec3 midpt = glm::vec3(0);

	// Point displacements
	std::vector<glm::vec3> nd1_displ;
	std::vector<glm::vec3> nd2_displ;
	std::vector<glm::vec3> nd3_displ;
	std::vector<glm::vec3> nd4_displ;
	std::vector<glm::vec3> midpt_displ;

	// Point displacement magnitude
	std::vector<double> nd1_displ_mag;
	std::vector<double> nd2_displ_mag;
	std::vector<double> nd3_displ_mag;
	std::vector<double> nd4_displ_mag;
	std::vector<double> midpt_displ_mag;

};

class pulse_elementquad_list_store
{
public:
	unsigned int pulse_elementquad_count = 0;
	std::unordered_map<int, pulse_elementquad_store> pulse_elementquadMap; // Create an unordered_map to store quads with ID as key
	double max_quad_displ = 0.0; // Maximum quad displacement

	pulse_elementquad_list_store();
	~pulse_elementquad_list_store();
	void init(geom_parameters* geom_param_ptr);
	void clear_data();
	void add_pulse_elementquadrilateral(int& quad_id, pulse_node_store* nd1, pulse_node_store* nd2,
		pulse_node_store* nd3, pulse_node_store* nd4, glm::vec3& midpt, std::vector<glm::vec3>& midpt_displ,
		std::vector<double>& midpt_displ_mag);
	void set_buffer();
	void paint_pulse_elementquads(const int& dyn_index);
	void update_geometry_matrices(bool set_modelmatrix, bool set_pantranslation, bool set_rotatetranslation,
		bool set_zoomtranslation, bool set_transparency, bool set_deflscale);

private:
	geom_parameters* geom_param_ptr = nullptr;
	// Anti clockwise
	//   4___________3		
	//   |		     |     
	//   |	   m	 |
	//   |			 |    
	//   1___________2        

	dynamic_tri_list_store pulse_element_tris1m2; // Tri 1m2
	dynamic_tri_list_store pulse_element_tris2m3; // Tri 2m3
	dynamic_tri_list_store pulse_element_tris3m4; // Tri 3m4
	dynamic_tri_list_store pulse_element_tris4m1; // Tri 4m1

};
