#include "pulse_analysis_window.h"

pulse_analysis_window::pulse_analysis_window()
{
	// Empty constructor
}

pulse_analysis_window::~pulse_analysis_window()
{
	// Empty destructor
}

void pulse_analysis_window::init()
{
	is_show_window = false; // Solver window open event flag
	execute_pulse_analysis = false; // Main solver run event flag
	execute_pulse_open = false; // Solver window execute opening event flag
	execute_pulse_close = false; // Closing of solution window event flag
	
	// Pulse option string
	pulse_option_str.clear();
	pulse_option_str.push_back("Half sine pulse");
	pulse_option_str.push_back("Rectangular pulse");
	pulse_option_str.push_back("Triangular pulse");
	pulse_option_str.push_back("Step force with finite rise");
	pulse_option_str.push_back("Harmonic/ Periodic excitation");

	stopwatch.start();
	mode_result_str.clear(); // Remove the mode result list

	time_interval_atrun = 0.0; // Value of time interval used in the pulse response 
	time_step_count = 0;
	time_step = 0;
}

void pulse_analysis_window::render_window()
{
	if (is_show_window == false)
		return;

	ImGui::Begin("Pulse Response Solver");


	//_________________________________________________________________________________________
	// Add a Modal Analysis button
	if (ImGui::Button("Pulse Analysis"))
	{
		execute_pulse_analysis = true;
	}

	ImGui::Spacing();
	ImGui::Spacing();

	// Show Modal analysis results
//________________________________________________________________________________________
	ImGui::Text("Start Mode 1 Frequency = %.3f Hz", static_cast<float>(modal_first_frequency));
	ImGui::Text("End Mode %i Frequency = %.3f Hz", number_of_modes, static_cast<float>(modal_end_frequency));

	// Inputs
	// Text for total simulation time
	//_________________________________________________________________________________________
	// Input box to give input via text
	static bool totaltime_input_mode = false;
	static char totaltime_str[16] = ""; // buffer to store input load string
	static float totaltime_input = static_cast<float>(total_simulation_time); // buffer to store input load End Time

	// Button to switch to input mode
	if (!totaltime_input_mode)
	{
		if (ImGui::Button("Total simulation time"))
		{
			totaltime_input_mode = true;
			snprintf(totaltime_str, 16, "%.3f", totaltime_input); // set the buffer to current load End Time
		}
	}
	else // input mode
	{
		// Text box to input load start time
		ImGui::SetNextItemWidth(60.0f);
		if (ImGui::InputText("##InputTotalTime", totaltime_str, IM_ARRAYSIZE(totaltime_str), ImGuiInputTextFlags_CharsDecimal))
		{
			// convert the input string to int
			totaltime_input = static_cast<float>(atof(totaltime_str));
			// set the load start time to input value
			total_simulation_time = totaltime_input;
		}

		// Button to switch back to slider mode
		ImGui::SameLine();
		if (ImGui::Button("OK"))
		{
			totaltime_input_mode = false;
		}
	}

	// Text for Total simulation time
	ImGui::SameLine();
	ImGui::Text("Total simulation time = %.3f", total_simulation_time);

	// Text for time interval
	//_________________________________________________________________________________________
	// Input box to give input via text
	static bool timeinterval_input_mode = false;
	static char timeinterval_str[16] = ""; // buffer to store input load string
	static float timeinterval_input = static_cast<float>(time_interval); // buffer to store input load End Time

	// Button to switch to input mode
	if (!timeinterval_input_mode)
	{
		if (ImGui::Button("Time Interval"))
		{
			timeinterval_input_mode = true;
			snprintf(timeinterval_str, 16, "%.3f", timeinterval_input); // set the buffer to current load End Time
		}
	}
	else // input mode
	{
		// Text box to input load start time
		ImGui::SetNextItemWidth(60.0f);
		if (ImGui::InputText("##InputTimeInterval", timeinterval_str, IM_ARRAYSIZE(timeinterval_str), ImGuiInputTextFlags_CharsDecimal))
		{
			// convert the input string to int
			timeinterval_input = static_cast<float>(atof(timeinterval_str));
			// set the load start time to input value
			time_interval = timeinterval_input;
		}

		// Button to switch back to slider mode
		ImGui::SameLine();
		if (ImGui::Button("OK"))
		{
			timeinterval_input_mode = false;
		}
	}


	// Text for Time Interval
	ImGui::SameLine();
	ImGui::Text("Time Interval = %.3f", time_interval);
	//_________________________________________________________________________________________

	// Dropdown list 3 Pulse Type
	// Add the pulse type dropdown list
	std::vector<const char*> items_pulse_cstr;
	for (const auto& item : pulse_option_str)
	{
		items_pulse_cstr.push_back(item.c_str());
	}

	const char* current_item3 = (selected_pulse_option >= 0 && selected_pulse_option < items_pulse_cstr.size()) ? items_pulse_cstr[selected_pulse_option] : "";

	if (ImGui::BeginCombo("Pulse Option", current_item3))
	{
		for (int i = 0; i < items_pulse_cstr.size(); i++)
		{
			const bool is_selected = (selected_pulse_option == i);
			if (ImGui::Selectable(items_pulse_cstr[i], is_selected))
			{
				selected_pulse_option = i;
			}

			// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
			if (is_selected)
			{
				ImGui::SetItemDefaultFocus();
			}
		}

		ImGui::EndCombo();
	}

	//_________________________________________________________________________________________
	// Add check boxes to show the Deformed model
	ImGui::Checkbox("Show Model", &show_undeformed_model);
	ImGui::Checkbox("Show Result Nodes", &show_result_nodes);
	ImGui::Checkbox("Show Result Lines", &show_result_lines);
	ImGui::Checkbox("Show Result Elements", &show_result_quads);


	ImGui::Spacing();
	//_________________________________________________________________________________________

	// Input box to give input via text
	static bool defscale_input_mode = false;
	static char defscale_str[16] = ""; // buffer to store input deformation scale string
	static double defscale_input = 0; // buffer to store input deformation scale value

	// Button to switch to input mode
	if (!defscale_input_mode)
	{
		if (ImGui::Button("Deformation Scale"))
		{
			defscale_input_mode = true;
			snprintf(defscale_str, 16, "%.4f", deformation_scale_max); // set the buffer to current deformation scale value
		}
	}
	else // input mode
	{
		// Text box to input value
		ImGui::SetNextItemWidth(60.0f);
		if (ImGui::InputText("##Deformation Scale", defscale_str, IM_ARRAYSIZE(defscale_str), ImGuiInputTextFlags_CharsDecimal))
		{
			// convert the input string to int
			defscale_input = atoi(defscale_str);
			// set the load value to input value
			deformation_scale_max = defscale_input;
		}

		// Button to switch back to slider mode
		ImGui::SameLine();
		if (ImGui::Button("OK"))
		{
			defscale_input_mode = false;
		}
	}

	// Text for load value
	ImGui::SameLine();
	ImGui::Text("%.3f", deformation_scale_max);

	// Slider for Deflection scale
	float deformation_scale_flt = static_cast<float>(deformation_scale_max);

	ImGui::Text("Deformation Scale");
	ImGui::SameLine();
	ImGui::SliderFloat(".", &deformation_scale_flt, 0.0f, 100.0f, "%.3f");
	deformation_scale_max = deformation_scale_flt;

	////Set the deformation scale
	//normailzed_defomation_scale = 1.0f;
	//deformation_scale = deformation_scale_max;

	ImGui::Spacing();
	//_________________________________________________________________________________________

	if (ImGui::CollapsingHeader("Animate", ImGuiTreeNodeFlags_DefaultOpen))
	{
		// Animate the solution
		// Start a horizontal layout
		ImGui::BeginGroup();

		// Play button active
		if (animate_pause == true)
		{
			// Add the Play button
			if (ImGui::Button("Play"))
			{
				// Handle Play button click
				animate_play = !animate_play;
				animate_pause = false;
			}
		}

		if (animate_play == true)
		{
			// Add the Pause button
			if (ImGui::Button("Pause"))
			{
				// Handle Pause button click
				animate_pause = !animate_pause;
				animate_play = false;
			}
		}

		// Add some spacing between buttons
		ImGui::SameLine();

		// Add the Stop button
		if (ImGui::Button("Stop"))
		{
			// Handle Stop button click
			animate_play = false;
			animate_pause = true;
			time_step = time_step_count-1;
		}

		// Animation speed control
		// Input box to give input via text
		static bool animation_speed_input_mode = false;
		static char animation_speed_str[16] = ""; // buffer to store input deformation scale string
		static float animation_speed_input = 0; // buffer to store input deformation scale value

		// Button to switch to input mode
		if (!animation_speed_input_mode)
		{
			if (ImGui::Button("Animation Speed"))
			{
				animation_speed_input_mode = true;
				snprintf(animation_speed_str, 16, "%.3f", animation_speed); // set the buffer to current deformation scale value
			}
		}
		else // input mode
		{
			// Text box to input value
			ImGui::SetNextItemWidth(60.0f);
			if (ImGui::InputText("##Animation Speed", animation_speed_str, IM_ARRAYSIZE(animation_speed_str), ImGuiInputTextFlags_CharsDecimal))
			{
				// convert the input string to int
				animation_speed_input = static_cast<float>(atof(animation_speed_str));
				// set the load value to input value
				animation_speed = animation_speed_input;
			}

			// Button to switch back to slider mode
			ImGui::SameLine();
			if (ImGui::Button("OK"))
			{
				animation_speed_input_mode = false;
			}
		}

		// Text for Animation speed value
		ImGui::SameLine();
		ImGui::Text(" %.3f", animation_speed);

		// Display the time step and time value
		ImGui::Text("Time = %.3f secs",
			time_interval_atrun* time_step);

		// Display the frame rate
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
			1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

		// End the horizontal layout
		ImGui::EndGroup();

	}

	ImGui::Spacing();
	//_________________________________________________________________________________________

		// Close button
	if (ImGui::Button("Close"))
	{
		execute_pulse_close = true;
		execute_pulse_open = false;
		is_show_window = false; // set the flag to close the window
	}

	ImGui::End();

	// Cycle through the pulse response time step
	if (time_step_count != 0)
	{
		if (animate_play == true)
		{
			// Stop watch
			if ((stopwatch.elapsed() * animation_speed) > time_interval_atrun)
			{
				stopwatch.start(); // Re-start the time
				time_step++; // increment the time step

				// Adjust the time step such that it didnot exceed the time_step_total
				if (time_step >= time_step_count)
				{
					time_step = 0;
				}
			}
		}
		else if (animate_pause == true)
		{
			// Pause or stop the animation
		
		}
	}


}
