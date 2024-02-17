#pragma once
#include "geom_parameters.h"

// File system
#include <fstream>
#include <sstream>
#include <iomanip>

// Window includes
#include "../tool_window/new_model_window.h"
#include "../tool_window/node_load_window.h"
#include "../tool_window/inlcondition_window.h"
#include "../tool_window/options_window.h"
#include "../tool_window/modal_analysis_window.h"
#include "../tool_window/pulse_analysis_window.h"

// Solver
#include "../fe_solver/modal_analysis_solver.h"
#include "../fe_solver/pulse_analysis_solver.h"

// FE Objects
#include "fe_objects/nodes_list_store.h"
#include "fe_objects/elementline_list_store.h"
#include "fe_objects/elementtri_list_store.h"
#include "fe_objects/elementquad_list_store.h"
#include "fe_objects/nodeload_list_store.h"
#include "fe_objects/nodeinlcond_list_store.h"

// Geometry Objects
#include "geometry_objects/dynamic_selrectangle_store.h"

// FE Result Objects Modal analysis
#include "result_objects/modal_nodes_list_store.h";
#include "result_objects/modal_elementline_list_store.h";
#include "result_objects/modal_elementtri_list_store.h";
#include "result_objects/modal_elementquad_list_store.h"

// FE Result Objects Pulse analysis
#include "result_objects/pulse_node_list_store.h"
#include "result_objects/pulse_elementline_list_store.h"
#include "result_objects/pulse_elementtri_list_store.h"
#include "result_objects/pulse_elementquad_list_store.h"


class geom_store
{
public: 
	const double m_pi = 3.14159265358979323846;
	bool is_geometry_set = false;

	// Main Variable to strore the geometry parameters
	geom_parameters geom_param;

	geom_store();
	~geom_store();

	void init(modal_analysis_window* modal_solver_window,
		pulse_analysis_window* pulse_solver_window,
		options_window* op_window,
		node_load_window* nd_load_window, 
		inlcondition_window* nd_inlcond_window,
		new_model_window* md_window);
	void fini();

	// Load the geometry
	void load_model(const int& model_type,std::vector<std::string> input_data);
	std::vector<elementline_store> removeDuplicates(const std::vector<elementline_store>& elementline);

	// Functions to control the drawing area
	void update_WindowDimension(const int& window_width, const int& window_height);
	void update_model_matrix();
	void update_model_zoomfit();
	void update_model_pan(glm::vec2& transl);
	void update_model_rotate(glm::mat4& rotation_m);
	void update_model_zoom(double& z_scale);
	void update_model_transperency(bool is_transparent);

	// Function to paint the selection rectangle
	void update_selection_rectangle(const glm::vec2& o_pt, const glm::vec2& c_pt,
		const bool& is_paint, const bool& is_select, const bool& is_rightbutton);

	// Functions to paint the geometry and results
	void paint_geometry();
private:
	dynamic_selrectangle_store selection_rectangle;

	// Geometry objects
	nodes_list_store model_nodes;
	elementline_list_store model_lineelements;
	elementtri_list_store model_trielements;
	elementquad_list_store model_quadelements;

	// Material data
	material_data mat_data;

	// Node initial condition, loads & Constraints
	nodeload_list_store node_loads;
	nodeinlcond_list_store node_inldispl;
	nodeinlcond_list_store node_inlvelo;

	// Modal analysis result 
	modal_nodes_list_store modal_result_nodes;
	modal_elementline_list_store modal_result_lineelements;
	modal_elementtri_list_store modal_result_trielements;
	modal_elementquad_list_store modal_result_quadelements;

	// Pulse analysis result
	pulse_node_list_store pulse_result_nodes;
	pulse_elementline_list_store pulse_result_lineelements;
	pulse_elementtri_list_store pulse_result_trielements;
	pulse_elementquad_list_store pulse_result_quadelements;

	// Solver object
	modal_analysis_solver modal_solver;
	pulse_analysis_solver pulse_solver;

	// Window pointers
	new_model_window* md_window = nullptr;
	options_window* op_window = nullptr;
	node_load_window* nd_load_window = nullptr;
	inlcondition_window* nd_inlcond_window = nullptr;

	// Analysis window
	modal_analysis_window* modal_solver_window = nullptr;
	pulse_analysis_window* pulse_solver_window = nullptr;

	void paint_model(); // Paint the model
	void paint_model_results(); // Paint the results

	//_____________________________________________________________________________________
	void paint_modal_analysis_results(); // Paint the modal analysis results
	void paint_pulse_analysis_results(); // Paint the pulse analysis results
	void paint_node_load_operation(); // Paint the node load window
	void paint_node_inlcond_operation(); // Paint the node initial condition window
};

