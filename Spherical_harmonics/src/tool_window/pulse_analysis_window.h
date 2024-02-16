#pragma once
#include <iostream>
#include <vector>
#include "../ImGui/imgui.h"
#include "../ImGui/imgui_impl_glfw.h"
#include "../ImGui/imgui_impl_opengl3.h"
#include "../ImGui/stb_implement.h"
#include "../geometry_store/geom_parameters.h"


class pulse_analysis_window
{
public:
	bool is_show_window = false;
	bool execute_pulse_analysis = false; // Main solver run event flag
	bool execute_pulse_open = false; // Solver window execute opening event flag
	bool execute_pulse_close = false; // Closing of solution window event flag

	std::vector<std::string> mode_result_str;
	std::vector<std::string> pulse_option_str;
	int selected_pulse_option = 0;

	// Inputs for response calculation
	double total_simulation_time = 10.0;
	double time_interval = 0.1;
	double damping_ratio = 0.01;

	// Modal analysis Results
	double modal_first_frequency = 0.0;
	double modal_end_frequency = 0.0;
	int number_of_modes = 0;

	// analysis results
	bool show_undeformed_model = true; // show undeformed model 
	bool show_result_nodes = true; // show result nodes
	bool show_result_lines = true; // show result elements
	bool show_result_quads = true; // show result quads

	double deformation_scale_max = 10.0;

	// Time step control
	double time_interval_atrun = 0.0; // Value of time interval used in the pulse response 
	int time_step_count = 0;
	int time_step = 0;

	pulse_analysis_window();
	~pulse_analysis_window();
	void init();// initialize
	void render_window();
private:
	Stopwatch_events stopwatch;

	// Animation control
	bool animate_play = true;
	bool animate_pause = false;
	double animation_speed = 1.0;

};
