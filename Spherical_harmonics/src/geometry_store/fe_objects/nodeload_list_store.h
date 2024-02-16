#pragma once
#include "nodes_list_store.h"

struct load_data
{
	int load_id = 0; // Load id
	int node_id = 0; // id of the node its applied to
	glm::vec3 load_loc = glm::vec3(0); // Load location
	double load_value = 0.0; // Load value
	double load_start_time = 0.0; // Load start time
	double load_end_time = 0.0; // Load end time
	bool show_load_label = false;

};

class nodeload_list_store
{
public:
	const double epsilon = 0.000001;
	int load_count = 0;
	std::unordered_map<int, load_data> loadMap;
	int model_type = 0; // 0 - Circle, 1,2,3 - Rectangle

	nodeload_list_store();
	~nodeload_list_store();
	void init(geom_parameters* geom_param_ptr);
	void set_zero_condition(const int& model_type);
	void add_loads(int& node_id, glm::vec3& load_loc, double& load_start_time, 
		double& load_end_time,double& load_value);
	void delete_load(int& node_id);
	void set_buffer();
	void paint_loads();
	void paint_load_labels();
	void update_geometry_matrices(bool set_modelmatrix, bool set_pantranslation, bool set_rotatetranslation,
		bool set_zoomtranslation, bool set_transparency, bool set_deflscale);
private:
	geom_parameters* geom_param_ptr = nullptr;
	gBuffers load_buffer;
	Shader load_shader;
	label_list_store load_value_labels;
	double load_max = 0.0;
	std::vector<int> all_load_ids;

	void get_load_buffer(load_data& ld, float* load_vertices, unsigned int& load_v_index, unsigned int* load_indices, unsigned int& load_i_index);
	int get_unique_load_id(std::vector<int>& all_ids);
};
