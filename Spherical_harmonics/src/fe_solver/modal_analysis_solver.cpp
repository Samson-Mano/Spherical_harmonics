#include "modal_analysis_solver.h"

modal_analysis_solver::modal_analysis_solver()
{
	// Empty constructor
}

modal_analysis_solver::~modal_analysis_solver()
{
	// Empty destructor
}


void modal_analysis_solver::clear_results()
{
	// Clear the eigen values and eigen vectors
	nodeid_map.clear(); // Node ID map
	number_of_modes = 0;
	node_count = 0;

	mode_result_str.clear();
	is_modal_analysis_complete = false;
}


void modal_analysis_solver::modal_analysis_start(const nodes_list_store& model_nodes,
	const elementtri_list_store& model_trielements,
	const elementquad_list_store& model_quadelements,
	const material_data& mat_data,
	rslt_nodes_list_store& modal_result_nodes,
	rslt_elementtri_list_store& modal_result_trielements,
	rslt_elementquad_list_store& modal_result_quadelements)
{
	// Main solver call
	this->is_modal_analysis_complete = false;

	// Check the model
	// Number of nodes
	if (model_nodes.node_count == 0)
	{
		return;
	}

	// Number of quads/ tris
	if ((model_quadelements.elementquad_count  + model_trielements.elementtri_count)== 0)
	{
		return;
	}

	//____________________________________________
	Eigen::initParallel();  // Initialize Eigen's thread pool

	stopwatch.start();

	stopwatch_elapsed_str.str("");
	stopwatch_elapsed_str << std::fixed << std::setprecision(6);

	std::cout << "Modal analysis - started" << std::endl;
	//____________________________________________________________________________________________________________________


	this->node_count = model_nodes.node_count;
	this->model_type = mat_data.model_type;

	this->matrix_size = 0;

	// Clear the angular frequency matrix
	angular_freq_vector.resize(matrix_size);
	angular_freq_vector.setZero();

	// Clear the eigen values
	eigen_values_vector.resize(matrix_size);
	eigen_values_vector.setZero();

	// Clear the eigen vectors
	eigen_vectors_matrix.resize(matrix_size, matrix_size);
	eigen_vectors_matrix.setZero();

	// Re-initialize the variables
	modal_result_nodes.clear_results();
	modal_result_trielements.clear_results();
	modal_result_quadelements.clear_results();

	// Mode results as string
	mode_result_str.clear();

	if (paint_mode_count > matrix_size)
	{
		paint_mode_count = matrix_size;
	}





	stopwatch_elapsed_str.str("");
	stopwatch_elapsed_str << stopwatch.elapsed();
	std::cout << "Modal analysis complete at " << stopwatch_elapsed_str.str() << " secs" << std::endl;


}

