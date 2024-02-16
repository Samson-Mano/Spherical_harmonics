#pragma once
#include <iostream>
#include "../geometry_store/geom_parameters.h"
#include "../ImGui/imgui.h"
#include "../ImGui/imgui_impl_glfw.h"
#include "../ImGui/imgui_impl_opengl3.h"


class node_load_window
{
public:
	bool is_show_window = false;
	bool is_selection_changed = false;
	bool is_selected_count = false;

	bool apply_nodal_load = false;// Apply nodal load
	bool delete_nodal_load = false; // Delete nodal load
	std::vector<int> selected_nodes;

	double load_amplitude = 100.0; // load value
	double load_start_time = 0.2; // load start time
	double load_end_time = 0.6; // load end time

	node_load_window();
	~node_load_window();
	void init(); // initialize bind images
	void render_window();
	void add_to_node_list(const std::vector<int>& selected_nodes, const bool& is_right);
private:
	
	void get_load_value_input();
	void get_load_starttime_input();
	void get_load_endtime_input();

};