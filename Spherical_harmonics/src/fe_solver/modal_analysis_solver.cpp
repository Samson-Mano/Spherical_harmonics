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

	this->matrix_size = model_nodes.node_count;

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

	// Legendre polynomial
	set_legendre_polynomial();




	stopwatch_elapsed_str.str("");
	stopwatch_elapsed_str << stopwatch.elapsed();
	std::cout << "Modal analysis complete at " << stopwatch_elapsed_str.str() << " secs" << std::endl;


}


void modal_analysis_solver::set_legendre_polynomial()
{
	// Set the legendre polynomial

	std::ofstream output_file;
	output_file.open("modal_analysis_results.txt");

	//output_file << "Eigen vector matrix:" << std::endl;
	//output_file << this->eigen_vectors_matrix << std::endl;
	//output_file << std::endl;


	output_file << "Number of nodes = " << matrix_size << std::endl;


	// Create the associated legendre polynomial values
	eigen_freq.clear();
	int mode_count = 0;

	for (int i = 1; i <= matrix_size; i++)
	{
		for (int j = -i; j < (i + 1); j++)
		{
			// Create the legendre polynomial
			legendre_polynomial_function temp_p;
			temp_p.mode_number = (mode_count + 1);
			temp_p.l = i;
			temp_p.m = j;

			output_file << "mode number = " << (mode_count + 1) << ", l = " << i << ", m = " << j << std::endl;

			// Add to the list
			eigen_freq.push_back(temp_p);

			mode_count++;

			if (mode_count >= matrix_size)
			{
				// Exit both the for loops
				break;
			}

		}

		if (mode_count >= matrix_size)
		{
			// Exit both the for loops
			break;
		}
	}

	output_file.close();

}


double modal_analysis_solver::legendre_polynomial(const int& l_param, const int& m_param)
{
	return 0.0;

}