#include "options_window.h"

options_window::options_window()
{
	// Empty constructor
}

options_window::~options_window()
{
	// Empty destructor
}

void options_window::init()
{
	// Initialize the options
	is_show_loads = true; // Show loads
	is_show_inlcondition = true; // show initial condition

	// Model Nodes
	is_show_modelnodes = false; // Show model nodes

	// Model elements
	is_show_modeledeges = false; // Show model edges
	is_show_modelelements = true; // Show model elements
	is_show_meshnormals = false; // Show mesh normals

	is_show_window = false;
}

void options_window::render_window()
{
	if (is_show_window == false)
		return;

	// Create a new ImGui options window
	ImGui::Begin("View Options");

	// Add Constraints check box
	ImGui::Text("Constraint options");
	ImGui::Checkbox("Show Loads", &is_show_loads);
	ImGui::Checkbox("Show Point initial condition", &is_show_inlcondition);
	ImGui::Spacing();

	// Model nodes
	ImGui::Text("Node options");
	ImGui::Checkbox("Show Nodes", &is_show_modelnodes);
	ImGui::Spacing();

	// Model elements
	ImGui::Text("Element options");
	ImGui::Checkbox("Show Mesh Elements", &is_show_modelelements);
	ImGui::Checkbox("Show Mesh Edges", &is_show_modeledeges);
	ImGui::Checkbox("Show Mesh Normals", &is_show_meshnormals);


	ImGui::Spacing();
	ImGui::Spacing();

	// Add a "Close" button
	if (ImGui::Button("Close"))
	{
		is_show_window = false;
	}

	ImGui::End();
}