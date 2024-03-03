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
	if ((model_quadelements.elementquad_count + model_trielements.elementtri_count) == 0)
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

	// Eigenvalue harmonic function
	// set_eigenvalue_harmonic_function();


	// Create the eigen vector node map
	int j = 0;
	int node_id = 0;
	this->nodeid_map.clear();

	for (auto& nd_m : model_nodes.nodeMap)
	{
		// Get the node id
		node_id = nd_m.first;

		this->nodeid_map[node_id] = j;
		j++;
	}


	//________________________________________________________________________________________________________________
	int num_intervals = this->matrix_size;
	double progress_interval = num_intervals / 10.0;
	int last_printed_progress = -1; // Initialize with an invalid value

	double c_radius = 100.0;
	double c_param = std::sqrt(mat_data.line_tension / mat_data.material_density);

	this->number_of_modes = 0;

	for (int l_param = 1; l_param <= this->matrix_size; l_param++)
	{
		// legendre l parameter
		for (int m_param = -l_param; m_param < (l_param + 1); m_param++)
		{
			// legendre m parameter

			// Variable to store eigen vectors at column
			Eigen::VectorXd eigen_vectors_at_column(this->matrix_size);
			eigen_vectors_at_column.setZero();

			// Node
			for (auto& nd_m : model_nodes.nodeMap)
			{
				// Get the node ID
				node_id = nd_m.first;

				// Node is not constrained, so get the co-ordinate
				glm::vec3 node_pt = nd_m.second.node_pt();

				int matrix_index = nodeid_map[node_id];

				// Eigen vectors
				double t_eigen_vec = get_spherical_eigen_vec(l_param, m_param, node_pt);

				// Add the eigen vectors
				eigen_vectors_at_column.coeffRef(matrix_index) = t_eigen_vec;
			}

			//_______________________________________________________________________________
			// Check whether the eigen vectors at column
			 bool has_invalid_value = false;

			// Check for invalid values
			for (int i = 0; i < static_cast<int>(eigen_vectors_at_column.size()); i++)
			{
				double value = eigen_vectors_at_column(i);

				if (std::isnan(value) || std::isinf(value)) 
				{
					has_invalid_value = true;
					break;
				}
			}

			if (has_invalid_value == true)
			{
				// Legendre polynomial function fails !!! Ignore this particular mode 
				// It is noticed that for certain l,m,x values associlated legendre polynomial function fails!!
				// example: l =29, m =29, x =0, l = 30, m =30, x = 0 etc

				//for (int i = 0; i < static_cast<int>(eigen_vectors_at_column.size()); i++)
				//{
				//	double value = eigen_vectors_at_column(i);

				//	if (std::isnan(value) || std::isinf(value))
				//	{
				//		eigen_vectors_at_column.coeffRef(i) = 1.0;

				//		// has_invalid_value = true;
				//		// break;
				//	}
				//	else
				//	{
				//		eigen_vectors_at_column.coeffRef(i) = 0.0;
				//	}
				//}

				// continue;
			}


			//________________________________________________________________________________
			// Normalize the eigen vector matrix
			double column_max = eigen_vectors_at_column.maxCoeff();

			if (std::fabs(column_max) < 1e-6) // || std::isnan(column_max) || std::isinf(column_max))
			{
				// column maximum is zero
				column_max = 1.0;
				eigen_vectors_at_column.setZero();

				// continue;
			}

			// Add to the eigen vector matrix
			eigen_vectors_matrix.col(this->number_of_modes) = eigen_vectors_at_column / column_max;


			//________________________________________________________________________________
			// Fill the eigen values
			double t_eigen = ((l_param * (l_param + 1)) / c_radius) * c_param;

			// Angular frequency wn
			angular_freq_vector.coeffRef(this->number_of_modes) = t_eigen;

			// Eigen value
			eigen_values_vector.coeffRef(this->number_of_modes) = (t_eigen * t_eigen);

			this->number_of_modes++;

			// Frequency
			double nat_freq = t_eigen / (2.0 * m_pi);

			if (this->number_of_modes <= paint_mode_count)
			{
				// Modal results
				std::stringstream ss;
				ss << std::fixed << std::setprecision(3) << nat_freq;

				// Add to the string list
				mode_result_str.push_back("Mode " + std::to_string(this->number_of_modes) + " = " + ss.str() + " Hz (l ="
					+ std::to_string(l_param) + ", m =" + std::to_string(m_param) + ")");
			}

			// Calculate percentage progress
			int progress_percentage = static_cast<int>((this->number_of_modes / static_cast<float>(this->matrix_size)) * 100);
			// Check if it's a new 10% interval
			if ((progress_percentage / 10) > last_printed_progress)
			{
				stopwatch_elapsed_str.str("");
				stopwatch_elapsed_str << stopwatch.elapsed();
				std::cout << progress_percentage << "% modal analysis completed at " << stopwatch_elapsed_str.str() << " secs" << std::endl;
				last_printed_progress = (progress_percentage / 10);
			}


			if (this->number_of_modes >= this->matrix_size)
			{
				// Exit both the for loops
				break;
			}
		}

		if (this->number_of_modes >= this->matrix_size)
		{
			// Exit both the for loops
			break;
		}
	}


	//_____________________________________________________________________________________
	// Map the results

	for (auto& nd_m : model_nodes.nodeMap)
	{
		node_id = nd_m.first;
		glm::vec3 node_pt = nd_m.second.node_pt();

		// Modal analysis results
		std::vector<glm::vec3> node_modal_displ;
		std::vector<double> node_modal_displ_magnitude;

		// Check whether the node is fixed or not
		int matrix_index = nodeid_map[node_id];

		for (int i = 0; i < paint_mode_count; i++)
		{
			double displ_magnitude = static_cast<float>(eigen_vectors_matrix.coeff(matrix_index, i));

			// get the appropriate modal displacement of this particular node
			glm::vec3 modal_displ = glm::normalize(node_pt) * static_cast<float>(displ_magnitude);

			// add to modal result of this node
			node_modal_displ.push_back(modal_displ);
			node_modal_displ_magnitude.push_back(std::abs(displ_magnitude));
		}


		// Create the modal analysis result node
		modal_result_nodes.add_result_node(node_id, node_pt, node_modal_displ, node_modal_displ_magnitude);
	}

	// Set the maximum displacement
	double max_displacement = 1.0;
	modal_result_nodes.set_max_displacement(max_displacement);


	stopwatch_elapsed_str.str("");
	stopwatch_elapsed_str << stopwatch.elapsed();
	std::cout << "Results mapped to model nodes at " << stopwatch_elapsed_str.str() << " secs" << std::endl;
	//____________________________________________________________________________________________________________________

	// Add the modal tri element result
	for (auto& tri_m : model_trielements.elementtriMap)
	{
		elementtri_store tri = tri_m.second;
		int tri_id = tri.tri_id;

		modal_result_trielements.add_rslt_elementtriangle(tri_id,
			&modal_result_nodes.rslt_nodeMap[tri.nd1->node_id],
			&modal_result_nodes.rslt_nodeMap[tri.nd2->node_id],
			&modal_result_nodes.rslt_nodeMap[tri.nd3->node_id]);
	}


	stopwatch_elapsed_str.str("");
	stopwatch_elapsed_str << stopwatch.elapsed();
	std::cout << "Results mapped to model Tri Elements at " << stopwatch_elapsed_str.str() << " secs" << std::endl;
	//____________________________________________________________________________________________________________________

	// Add the modal quad element result
	for (auto& quad_m : model_quadelements.elementquadMap)
	{
		elementquad_store quad = quad_m.second;
		int quad_id = quad.quad_id;

		modal_result_quadelements.add_rslt_elementquadrilateral(quad_id,
			&modal_result_nodes.rslt_nodeMap[quad.nd1->node_id],
			&modal_result_nodes.rslt_nodeMap[quad.nd2->node_id],
			&modal_result_nodes.rslt_nodeMap[quad.nd3->node_id],
			&modal_result_nodes.rslt_nodeMap[quad.nd4->node_id]);
	}


	stopwatch_elapsed_str.str("");
	stopwatch_elapsed_str << stopwatch.elapsed();
	std::cout << "Results mapped to model Quad Elements at " << stopwatch_elapsed_str.str() << " secs" << std::endl;
	//____________________________________________________________________________________________________________________

	this->is_modal_analysis_complete = true;

	stopwatch_elapsed_str.str("");
	stopwatch_elapsed_str << stopwatch.elapsed();
	std::cout << "Modal analysis complete at " << stopwatch_elapsed_str.str() << " secs" << std::endl;


}


//void modal_analysis_solver::set_eigenvalue_harmonic_function()
//{
//	// Set the legendre polynomial
//
//	std::ofstream output_file;
//	output_file.open("modal_analysis_results.txt");
//
//	//output_file << "Eigen vector matrix:" << std::endl;
//	//output_file << this->eigen_vectors_matrix << std::endl;
//	//output_file << std::endl;
//
//
//	output_file << "Number of nodes = " << matrix_size << std::endl;
//
//
//	// Create the eigenvalue harmonic function values
//	eigen_freq.clear();
//	int mode_count = 0;
//
//	for (int i = 1; i <= matrix_size; i++)
//	{
//		for (int j = -i; j < (i + 1); j++)
//		{
//			// Create the eigenvalue harmonic function
//			eigenvalue_harmonic_function temp_p;
//			temp_p.mode_number = (mode_count + 1);
//			temp_p.l = i;
//			temp_p.m = j;
//			temp_p.root_value = i;
//
//			output_file << "mode number = " << (mode_count + 1) << ", l = " << i << ", m = " << j << std::endl;
//
//			// Add to the list
//			eigen_freq.push_back(temp_p);
//
//			mode_count++;
//
//			if (mode_count >= matrix_size)
//			{
//				// Exit both the for loops
//				break;
//			}
//
//		}
//
//		if (mode_count >= matrix_size)
//		{
//			// Exit both the for loops
//			break;
//		}
//	}
//
//	output_file.close();
//
//}

double modal_analysis_solver::get_spherical_eigen_vec(const int& l_param, const int& m_param, const glm::vec3& node_pt)
{
	double nd_theta = std::acos(node_pt.x/100.0); // angle made in xz plane
	double nd_phi = std::atan2(node_pt.y,node_pt.z); // angle made in xy plane

	if (m_param < -0.1)
	{
		// Sin parameter (when m is negative)
		return std::sin(-1.0f * m_param * nd_phi) * legendre_polynomial(l_param, m_param, nd_theta);

	}
	else
	{
		// cos paramter (when m is positive)
		return std::cos(m_param * nd_phi) *  legendre_polynomial(l_param, m_param, nd_theta);

	}
}


double modal_analysis_solver::legendre_polynomial(const int& l_param, const int& m_param, const double& theta)
{
	// Cos theta (theta in radians)
	float x_val = std::cos(theta);

	return std::assoc_legendre(l_param, m_param, x_val);

}