#include "inlcondition_window.h"

inlcondition_window::inlcondition_window()
{
	// Empty Constructor
}

inlcondition_window::~inlcondition_window()
{
	// Empty Destructor
}

void inlcondition_window::init()
{
	is_show_window = false;
}

void inlcondition_window::render_window()
{
	if (is_show_window == false)
		return;

	ImGui::Begin("Initial Condition");


	// Options for the Initial condition type (Displacement or Velocity)
	if (ImGui::RadioButton("Set Initial Displacement", selected_inl_option == 0))
	{
		selected_inl_option = 0;
	}
	// ImGui::SameLine();
	if (ImGui::RadioButton("Set Initial Velocity", selected_inl_option == 1))
	{
		selected_inl_option = 1;
	}


	// ________________________________________________________________________________________________________________________________

	ImGui::Spacing();
	ImGui::Spacing();

	if (selected_inl_option == 0)
	{
		ImGui::Text("Initial Displacement");
		// ________________________________________________________________________________________________________________________________

		get_idisplz_value_input();

	}
	else if (selected_inl_option == 1)
	{
		ImGui::Text("Initial Velocity");
		// ________________________________________________________________________________________________________________________________

		get_iveloz_value_input();
	}


	
	// ________________________________________________________________________________________________________________________________
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
		apply_nodal_inlcond = true; // set the flag to apply the initial condition
	}

	ImGui::SameLine();

	// Delete load button
	if (ImGui::Button("Delete"))
	{
		delete_nodal_inlcond = true; // set the flag to delete the initial condition
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

		apply_nodal_inlcond = false;
		delete_nodal_inlcond = false;
		is_show_window = false; // set the flag to close the window
	}

	ImGui::End();
}

void inlcondition_window::add_to_node_list(const std::vector<int>& selected_nodes, const bool& is_right)
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

void inlcondition_window::get_idisplz_value_input()
{
	// Input box to give input via text
	static bool displz_input_mode = false;
	static char displz_str[16] = ""; // buffer to store input Displacement Z string
	static float displz_input = 0; // buffer to store input Displacement Z value

	// Button to switch to input mode
	if (!displz_input_mode)
	{
		if (ImGui::Button("Displacmenet"))
		{
			displz_input_mode = true;
			snprintf(displz_str, 16, "%.1f", initial_displacement_z); // set the buffer to current Displacement Z
		}
	}
	else // input mode
	{
		// Text box to input value
		ImGui::SetNextItemWidth(60.0f);
		if (ImGui::InputText("##DisplacementX", displz_str, IM_ARRAYSIZE(displz_str), ImGuiInputTextFlags_CharsDecimal))
		{
			// convert the input string to int
			initial_displacement_z = atof(displz_str);
			// set the load value to input value
			// deformation_scale_max = defscale_input;
		}

		// Button to switch back to slider mode
		ImGui::SameLine();
		if (ImGui::Button("OK"))
		{
			displz_input_mode = false;
		}
	}

	// Text for load value
	ImGui::SameLine();
	ImGui::Text(" %.1f", initial_displacement_z);

}


void inlcondition_window::get_iveloz_value_input()
{
	// Input box to give input via text
	static bool veloz_input_mode = false;
	static char veloz_str[16] = ""; // buffer to store input Velocity Z string
	static float veloz_input = 0; // buffer to store input Velocity Z value

	// Button to switch to input mode
	if (!veloz_input_mode)
	{
		if (ImGui::Button("Velocity"))
		{
			veloz_input_mode = true;
			snprintf(veloz_str, 16, "%.1f", initial_velocity_z); // set the buffer to current Velocity Z
		}
	}
	else // input mode
	{
		// Text box to input value
		ImGui::SetNextItemWidth(60.0f);
		if (ImGui::InputText("##VelocityZ", veloz_str, IM_ARRAYSIZE(veloz_str), ImGuiInputTextFlags_CharsDecimal))
		{
			// convert the input string to int
			initial_velocity_z = atof(veloz_str);
			// set the load value to input value
			// deformation_scale_max = defscale_input;
		}

		// Button to switch back to slider mode
		ImGui::SameLine();
		if (ImGui::Button("OK"))
		{
			veloz_input_mode = false;
		}
	}

	// Text for load value
	ImGui::SameLine();
	ImGui::Text(" %.1f", initial_velocity_z);
}
