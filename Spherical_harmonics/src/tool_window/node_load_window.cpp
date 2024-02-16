#include "node_load_window.h"

node_load_window::node_load_window()
{
	// Empty constructor
}

node_load_window::~node_load_window()
{
	// Empty destructor
}

void node_load_window::init()
{
	// Initialize

}

void node_load_window::render_window()
{
	if (is_show_window == false)
		return;

	ImGui::Begin("Loads");

	//_________________________________________________________________________________________
	// Get the load value input

	get_load_value_input();

	//_________________________________________________________________________________________
	// Get the load start time input

	get_load_starttime_input();

	//_________________________________________________________________________________________
	// Get the load end time input

	get_load_endtime_input();

	// Display the Load frequency
	ImGui::Text("Load frequency = %.3f Hz", 1.0f / (load_end_time - load_start_time));

	//_________________________________________________________________________________________

	// Selected Node list
	ImGui::Spacing();

	static char nodeNumbers[1024] = ""; // Increase the buffer size to accommodate more characters

	geom_parameters::copyNodenumberlistToCharArray(selected_nodes, nodeNumbers, 1024);

	ImGui::Text("Selected Nodes: ");
	ImGui::Spacing();

	// Begin a child window with ImGuiWindowFlags_HorizontalScrollbar to enable vertical scrollbar ImGuiWindowFlags_AlwaysVerticalScrollbar
	ImGui::BeginChild("Node Numbers", ImVec2(-1.0f, ImGui::GetTextLineHeight() * 10), true);

	// Assuming 'nodeNumbers' is a char array or a string
	ImGui::TextWrapped("%s", nodeNumbers);

	// End the child window
	ImGui::EndChild();

	//__________________________________________________________________________________________
	// Apply and Delete Button
	// Apply load button
	if (ImGui::Button("Apply"))
	{
		apply_nodal_load = true; // set the flag to apply to the load
	}

	ImGui::SameLine();

	// Delete load button
	if (ImGui::Button("Delete"))
	{
		delete_nodal_load = true; // set the flag to apply to the load
	}

	//__________________________________________________________________________________________
	ImGui::Spacing();

	// Close button
	if (ImGui::Button("Close"))
	{
		// Clear the selected nodes
		this->selected_nodes.clear();
		is_selected_count = false; // Number of selected nodes 0
		is_selection_changed = false; // Set the selection changed

		apply_nodal_load = false;
		delete_nodal_load = false;
		is_show_window = false; // set the flag to close the window
	}

	//__________________________________________________________________________________________

	ImGui::End();
}


void node_load_window::get_load_value_input()
{
	// Add Load input controls
	// Input box to give input via text
	static bool loadval_input_mode = false;
	static char load_str[16] = ""; // buffer to store input load string
	static float load_input = static_cast<float>(load_amplitude); // buffer to store input load value

	// Button to switch to input mode
	if (!loadval_input_mode)
	{
		if (ImGui::Button("Input Load"))
		{
			loadval_input_mode = true;
			snprintf(load_str, 16, "%.2f", load_input); // set the buffer to current load value
		}
	}
	else // input mode
	{
		// Text box to input load value
		ImGui::SetNextItemWidth(60.0f);
		if (ImGui::InputText("##InputLoad", load_str, IM_ARRAYSIZE(load_str), ImGuiInputTextFlags_CharsDecimal))
		{
			// convert the input string to int
			load_input = static_cast<float>(atof(load_str));
			// set the load value to input value
			load_amplitude = load_input;
		}

		// Button to switch back to slider mode
		ImGui::SameLine();
		if (ImGui::Button("OK"))
		{
			loadval_input_mode = false;
		}
	}

	// Text for load value
	ImGui::SameLine();
	ImGui::Text("Load = %.2f", load_input);
}


void node_load_window::get_load_starttime_input()
{
	// Input box to give input via text
	static bool loadstarttime_input_mode = false;
	static char loadstarttime_str[16] = ""; // buffer to store input load string
	static float loadstarttime_input = static_cast<float>(load_start_time); // buffer to store input load start time

	// Button to switch to input mode
	if (!loadstarttime_input_mode)
	{
		if (ImGui::Button("Input Start Time"))
		{
			loadstarttime_input_mode = true;
			snprintf(loadstarttime_str, 16, "%.3f", loadstarttime_input); // set the buffer to current load start time
		}
	}
	else // input mode
	{
		// Text box to input load start time
		ImGui::SetNextItemWidth(60.0f);
		if (ImGui::InputText("##InputStartTime", loadstarttime_str, IM_ARRAYSIZE(loadstarttime_str), ImGuiInputTextFlags_CharsDecimal))
		{
			// convert the input string to int
			loadstarttime_input = static_cast<float>(atof(loadstarttime_str));
			// set the load start time to input value
			load_start_time = loadstarttime_input;
		}

		// Button to switch back to slider mode
		ImGui::SameLine();
		if (ImGui::Button("OK"))
		{
			loadstarttime_input_mode = false;
		}
	}

	// Text for load start time
	ImGui::SameLine();
	ImGui::Text("Start Time = %.3f", loadstarttime_input);
}


void node_load_window::get_load_endtime_input()
{
	// Input box to give input via text
	static bool loadendtime_input_mode = false;
	static char loadendtime_str[16] = ""; // buffer to store input load string
	static float loadendtime_input = static_cast<float>(load_end_time); // buffer to store input load End Time

	// Button to switch to input mode
	if (!loadendtime_input_mode)
	{
		if (ImGui::Button("Input End Time"))
		{
			loadendtime_input_mode = true;
			snprintf(loadendtime_str, 16, "%.3f", loadendtime_input); // set the buffer to current load End Time
		}
	}
	else // input mode
	{
		// Text box to input load End Time
		ImGui::SetNextItemWidth(60.0f);
		if (ImGui::InputText("##InputEndTime", loadendtime_str, IM_ARRAYSIZE(loadendtime_str), ImGuiInputTextFlags_CharsDecimal))
		{
			// convert the input string to int
			loadendtime_input = static_cast<float>(atof(loadendtime_str));
			// set the load End Time to input value
			if (loadendtime_input > load_start_time)
			{
				load_end_time = loadendtime_input;
			}
			else
			{
				loadendtime_input = static_cast<float>(load_end_time);
			}
		}

		// Button to switch back to slider mode
		ImGui::SameLine();
		if (ImGui::Button("OK"))
		{
			loadendtime_input_mode = false;
		}
	}

	// Text for load End Time
	ImGui::SameLine();
	ImGui::Text("End Time = %.3f", loadendtime_input);
}

void node_load_window::add_to_node_list(const std::vector<int>& selected_nodes, const bool& is_right)
{
	if (is_right == false)
	{
		// Add to the selected node list
		for (int node : selected_nodes)
		{
			// Check whether nodes are already in the list or not
			if (std::find(this->selected_nodes.begin(), this->selected_nodes.end(), node) == this->selected_nodes.end())
			{
				// Add to selected nodes
				this->selected_nodes.push_back(node);

				// Selection changed flag
				this->is_selection_changed = true;
			}
		}
	}
	else
	{
		// Remove from the selected node list
		for (int node : selected_nodes)
		{
			// Erase the node which is found in the list
			this->selected_nodes.erase(std::remove(this->selected_nodes.begin(), this->selected_nodes.end(), node),
				this->selected_nodes.end());

			// Selection changed flag
			this->is_selection_changed = true;
		}

	}

	// Number of selected nodes
	this->is_selected_count = false;
	if (static_cast<int>(this->selected_nodes.size()) > 0)
	{
		this->is_selected_count = true;
	}

}
