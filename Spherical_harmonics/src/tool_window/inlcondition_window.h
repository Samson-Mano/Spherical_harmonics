#pragma once
#include <iostream>
#include "../geometry_store/geom_parameters.h"
#include "../ImGui/imgui.h"
#include "../ImGui/imgui_impl_glfw.h"
#include "../ImGui/imgui_impl_opengl3.h"

class inlcondition_window
{
public:
	bool is_show_window = false;
	bool is_selection_changed = false;
	bool is_selected_count = false;
	int selected_inl_option = 0; 	// Initial condition type 0 - Displacement, 1 - Velocity

	bool apply_nodal_inlcond = false;// Apply nodal initial condition
	bool delete_nodal_inlcond = false; // Delete nodal initial condition
	std::vector<int> selected_nodes;

	// Initial displacement
	double initial_displacement_z = 1.0;
	// Initial velocity
	double initial_velocity_z = 1.0;

	inlcondition_window();
	~inlcondition_window();
	void init();
	void render_window();
	void add_to_node_list(const std::vector<int>& selected_nodes, const bool& is_right);
private:
	void get_idisplz_value_input();
	void get_iveloz_value_input();

};