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
	is_show_constraint = true; // Show constraints
	is_show_loads = true; // Show loads
	is_show_loadvalues = true; // Show load values;
	is_show_ptmass = true; // Show point mass
	is_show_ptmass_labels = true; // show point mass labels
	is_show_inlcondition = true; // show initial condition

	// Model Nodes
	is_show_modelnodes = true; // Show model nodes
	is_show_modelnodeids = true; // Show model node ids 
	is_show_modelnodecoords = true; // Show model node co-ordinates

	// Model elements
	is_show_modeledeges = true; // Show model edges
	is_show_modelelements = true; // Show model elements

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
	ImGui::Checkbox("Show Constraints", &is_show_constraint);
	ImGui::Checkbox("Show Loads", &is_show_loads);
	ImGui::Checkbox("Show Load values", &is_show_loadvalues);
	ImGui::Checkbox("Show Point mass", &is_show_ptmass);
	ImGui::Checkbox("Show Point mass values", &is_show_ptmass_labels);
	ImGui::Checkbox("Show Point initial condition", &is_show_inlcondition);
	ImGui::Spacing();

	// Model nodes
	ImGui::Text("Node options");
	ImGui::Checkbox("Show Nodes", &is_show_modelnodes);
	ImGui::Checkbox("Show Node IDs", &is_show_modelnodeids);
	ImGui::Checkbox("Show Node Co-ordinates", &is_show_modelnodecoords);
	ImGui::Spacing();

	// Model elements
	ImGui::Text("Element options");
	ImGui::Checkbox("Show Elements", &is_show_modelelements);
	ImGui::Checkbox("Show Edgess", &is_show_modeledeges);

	ImGui::Spacing();
	ImGui::Spacing();

	// Add a "Close" button
	if (ImGui::Button("Close"))
	{
		is_show_window = false;
	}

	ImGui::End();
}