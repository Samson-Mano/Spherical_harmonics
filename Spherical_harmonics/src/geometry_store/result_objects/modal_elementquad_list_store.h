#pragma once
#include "modal_elementline_list_store.h"
#include "../geometry_objects/dynamic_tri_list_store.h"


struct modal_elementquad_store
{
	int quad_id = 0; // ID of the quadrilateral element
	modal_node_store* nd1 = nullptr; // node 1
	modal_node_store* nd2 = nullptr; // node 2
	modal_node_store* nd3 = nullptr; // node 3
	modal_node_store* nd4 = nullptr; // node 4

	// Point coordinate
	glm::vec3 nd1pt = glm::vec3(0);
	glm::vec3 nd2pt = glm::vec3(0);
	glm::vec3 nd3pt = glm::vec3(0);
	glm::vec3 nd4pt = glm::vec3(0);
	glm::vec3 midpt = glm::vec3(0);

	// Point displacements
	std::vector<glm::vec3> nd1_modal_displ;
	std::vector<glm::vec3> nd2_modal_displ;
	std::vector<glm::vec3> nd3_modal_displ;
	std::vector<glm::vec3> nd4_modal_displ;
	std::vector<glm::vec3> midpt_modal_displ;

	// Point displacement magnitude
	std::vector<double> nd1_modal_displ_mag;
	std::vector<double> nd2_modal_displ_mag;
	std::vector<double> nd3_modal_displ_mag;
	std::vector<double> nd4_modal_displ_mag;
	std::vector<double> midpt_modal_displ_mag;

};


class modal_elementquad_list_store
{
public:
	unsigned int modal_elementquad_count = 0;
	std::unordered_map<int, modal_elementquad_store> modal_elementquadMap; // Create an unordered_map to store Quadrilaterals with ID as key

	modal_elementquad_list_store();
	~modal_elementquad_list_store();
	void init(geom_parameters* geom_param_ptr, dcel_dynmesh_data* mesh_data);
	void add_modal_elementquadrilateral(int& quad_id, modal_node_store* nd1, modal_node_store* nd2,
		modal_node_store* nd3, modal_node_store* nd4, glm::vec3& midpt, std::vector<glm::vec3>& midpt_modal_displ,
		std::vector<double>& midpt_modal_displ_mag);

private:
	geom_parameters* geom_param_ptr = nullptr;
	dcel_dynmesh_data* mesh_data = nullptr;

};
