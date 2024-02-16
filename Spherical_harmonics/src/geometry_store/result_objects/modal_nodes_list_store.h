#pragma once
#include "../fe_objects/nodes_list_store.h"
#include "../geometry_objects/dynamic_point_list_store.h"

struct modal_node_store
{
	int node_id = 0;
	glm::vec3 node_pt = glm::vec3(0);

	// Modal results (x, y, z)
	std::vector<glm::vec3> node_modal_displ;
	std::vector<double> node_modal_displ_magnitude;
};

class modal_nodes_list_store
{
public:
	unsigned int node_count = 0;
	std::unordered_map<int, modal_node_store> modal_nodeMap; // Create an unordered_map to store nodes with ID as key

	modal_nodes_list_store();
	~modal_nodes_list_store();
	void init(geom_parameters* geom_param_ptr);
	void clear_data();
	void add_result_node(int& node_id,const glm::vec3& node_pt,const std::vector<glm::vec3>& node_modal_displ,
		const std::vector<double>& node_modal_displ_magnitude);
	void set_buffer();
	void update_buffer(const int& selected_mode);

	void paint_modal_nodes();
	void update_geometry_matrices(bool set_modelmatrix, bool set_pantranslation, bool set_rotatetranslation,
		bool set_zoomtranslation, bool set_transparency, bool set_deflscale);

private:
	geom_parameters* geom_param_ptr = nullptr;

	dynamic_point_list_store modal_node_points;

};
