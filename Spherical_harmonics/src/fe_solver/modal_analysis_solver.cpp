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
	constrained_node_map.clear(); // Constrained Node  map
	nodeid_map.clear(); // Node ID map
	number_of_modes = 0;
	node_count = 0;

	mode_result_str.clear();
	is_modal_analysis_complete = false;
}


void modal_analysis_solver::modal_analysis_start(const nodes_list_store& model_nodes,
	const elementline_list_store& model_lineelements,
	const elementtri_list_store& model_trielements,
	const elementquad_list_store& model_quadelements,
	const material_data& mat_data,
	modal_nodes_list_store& modal_result_nodes,
	modal_elementline_list_store& modal_result_lineelements,
	modal_elementtri_list_store& modal_result_trielements,
	modal_elementquad_list_store& modal_result_quadelements)
{
	// Main solver call
	this->is_modal_analysis_complete = false;

	// Check the model
	// Number of nodes
	if (model_nodes.node_count == 0)
	{
		return;
	}

	// Number of lines
	if (model_lineelements.elementline_count == 0)
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

	if (this->model_type == 0)
	{
		// Circular Membrane
		// Number of fixed nodes = 128 or 256 test = 16
		// Radius = 100
		this->matrix_size = node_count - 128;
	}
	else if (this->model_type == 1)
	{
		// Rectangular 1:1
		// Number of fixed nodes = 200

		this->matrix_size = node_count - 200;
	}
	else if (this->model_type == 2)
	{
		// Rectangular 1:2
		// Number of fixed nodes = 300
		this->matrix_size = node_count - 300;
	}
	else if (this->model_type == 3)
	{
		// Rectangular 1:3
		// Number of fixed nodes = 400
		this->matrix_size = node_count - 400;
	}
	else if(this->model_type == 4)
	{
		// Circular Membrane Triangles
		// Number of fixed nodes = 64
		// Radius = 100
		this->matrix_size = node_count - 64;
	}

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
	modal_result_nodes.clear_data();
	modal_result_lineelements.clear_data();
	modal_result_quadelements.clear_data();

	// Mode results as string
	mode_result_str.clear();

	if (paint_mode_count > matrix_size)
	{
		paint_mode_count = matrix_size;
	}


	if (mat_data.model_type == 0 || mat_data.model_type == 4)
	{
		// Circular membrane
		double c_radius = 100.0;

		modal_analysis_model_circular(model_nodes,
			model_lineelements,
			model_trielements,
			model_quadelements,
			mat_data,
			c_radius,
			modal_result_nodes,
			modal_result_lineelements,
			modal_result_trielements,
			modal_result_quadelements);

		this->is_modal_analysis_complete = true;

	}
	else if (mat_data.model_type == 1)
	{
		// Rectangular 1:1
		double length_x = 100.0;
		double length_y = 100.0;

		modal_analysis_model_rectangular(model_nodes,
			model_lineelements,
			model_quadelements,
			mat_data,
			length_x,
			length_y,
			modal_result_nodes,
			modal_result_lineelements,
			modal_result_quadelements);

		this->is_modal_analysis_complete = true;

	}
	else if (mat_data.model_type == 2)
	{
		// Rectangular 1:2
		double length_x = 100.0;
		double length_y = 200.0;

		modal_analysis_model_rectangular(model_nodes,
			model_lineelements,
			model_quadelements,
			mat_data,
			length_x,
			length_y,
			modal_result_nodes,
			modal_result_lineelements,
			modal_result_quadelements);

		this->is_modal_analysis_complete = true;

	}
	else if (mat_data.model_type == 3)
	{
		// Rectangular 1:3
		double length_x = 100.0;
		double length_y = 300.0;

		modal_analysis_model_rectangular(model_nodes,
			model_lineelements,
			model_quadelements,
			mat_data,
			length_x,
			length_y,
			modal_result_nodes,
			modal_result_lineelements,
			modal_result_quadelements);

		this->is_modal_analysis_complete = true;

	}

	stopwatch_elapsed_str.str("");
	stopwatch_elapsed_str << stopwatch.elapsed();
	std::cout << "Modal analysis complete at " << stopwatch_elapsed_str.str() << " secs" << std::endl;


}

void modal_analysis_solver::modal_analysis_model_circular(const nodes_list_store& model_nodes,
	const elementline_list_store& model_lineelements,
	const elementtri_list_store& model_trielements,
	const elementquad_list_store& model_quadelements,
	const material_data& mat_data,
	const double& c_radius,
	modal_nodes_list_store& modal_result_nodes,
	modal_elementline_list_store& modal_result_lineelements,
	modal_elementtri_list_store& modal_result_trielements,
	modal_elementquad_list_store& modal_result_quadelements)
{
	// Circular string
	int node_id = 0;
	double c_param = std::sqrt(mat_data.line_tension / mat_data.material_density);
	
	this->number_of_modes = 0;

	// Find the modal frequency
	int modal_m_count = std::ceilf(std::sqrt(this->matrix_size));

	// Bessel function Frequency
	std::vector<bessel_function_Frequency> t_bessel_roots;

	for (int m = 0; m < modal_m_count; m++)
	{
		for (int n = 0; n < modal_m_count; n++)
		{
			double bessel_value = boost::math::cyl_bessel_j_zero((float)m, (n+1),ignore_all_policy());
	
			// Temp 
			bessel_function_Frequency temp_bessel_roots;
			temp_bessel_roots.m = m;
			temp_bessel_roots.n = n;
			temp_bessel_roots.root_value = bessel_value;

			// Add to the list
			t_bessel_roots.push_back(temp_bessel_roots);
		}
	}

	// Sort the vector based on root_value
	std::sort(t_bessel_roots.begin(), t_bessel_roots.end(), compareRootValues);

	// Final Bessel function Frequency
	eigen_freq.clear();
	int temp_mode_number = 0;

	// std::ofstream outFile("bessel_roots.txt"); // Open a file for writing

	for (auto& b_root : t_bessel_roots)
	{
		if (b_root.m == 0)
		{
			// m = 0 has no phase change
			bessel_function_Frequency temp_bessel_roots;
			temp_bessel_roots.mode_number = temp_mode_number;
			temp_bessel_roots.m = b_root.m;
			temp_bessel_roots.n = b_root.n;
			temp_bessel_roots.phase_pi = 0.0; // Phase = 0 when m = 0
			temp_bessel_roots.root_value = b_root.root_value;

			// Add to the list
			eigen_freq.push_back(temp_bessel_roots);

			// outFile << temp_mode_number << ", " << b_root.m << ", "<< b_root.n << ", "<<b_root.root_value << std::endl; // Write to the file

			temp_mode_number++;
		}
		else
		{
			// m not equal to zero // so add the anti mode 
			bessel_function_Frequency temp_bessel_roots_p0;
			temp_bessel_roots_p0.mode_number = temp_mode_number;
			temp_bessel_roots_p0.m = b_root.m;
			temp_bessel_roots_p0.n = b_root.n;
			temp_bessel_roots_p0.phase_pi = 0.0; // Phase = 0 when m = 0
			temp_bessel_roots_p0.root_value = b_root.root_value;

			// Add to the list
			eigen_freq.push_back(temp_bessel_roots_p0);

			// outFile << temp_mode_number << ", " << b_root.m << ", "<< b_root.n << ", "<<b_root.root_value << std::endl; // Write to the file

			temp_mode_number++;

			// Antimode with pi/2 * m phase
			bessel_function_Frequency temp_bessel_roots_p1;
			temp_bessel_roots_p1.mode_number = temp_mode_number;
			temp_bessel_roots_p1.m = b_root.m;
			temp_bessel_roots_p1.n = b_root.n;
			temp_bessel_roots_p1.phase_pi = (1.0 / static_cast<float>(b_root.m)) * (m_pi * 0.5); // Phase = (pi/2) * (1/m) when m not equal to 0
			temp_bessel_roots_p1.root_value = b_root.root_value;

			// Add to the list
			eigen_freq.push_back(temp_bessel_roots_p1);

			// outFile << temp_mode_number << ", " << b_root.m << ", "<< b_root.n << ", "<<b_root.root_value << std::endl; // Write to the file

			temp_mode_number++;

		}
		
	}

	// Resize the eigen freq size (to be inline with matrix size)
	eigen_freq.resize(this->matrix_size);


	// outFile.close(); // Close the file

	// Create the eigen vector node map
	int j = 0;
	this->constrained_node_map.clear();
	this->nodeid_map.clear();

	for (auto& nd_m : model_nodes.nodeMap)
	{
		node_id = nd_m.first;
		glm::vec3 node_pt = nd_m.second.node_pt;
		double nd_radius = glm::length(node_pt);

		if (nd_radius >= (c_radius- 0.1) )
		{
			// Zero for fixed nodes
			this->constrained_node_map[node_id] = true; // is constrained
			continue;
		}

		this->constrained_node_map[node_id] = false; // Node is not fixed
		this->nodeid_map[node_id] = j;
		j++;
	}

	//________________________________________________________________________________________________________________

	// Create the mode shapes
	this->number_of_modes = 0;

	std::unordered_map<int, quad_midnode_eigenvector_store> quad_midnode;

	// Create the quad mid nodes
	for (auto& quad_m : model_quadelements.elementquadMap)
	{
		elementquad_store quad = quad_m.second;

		// Get the mid point of the quad
		std::vector<glm::vec3> quad_corner_pts;

		quad_corner_pts.push_back(quad.nd1->node_pt); // quad point 1
		quad_corner_pts.push_back(quad.nd2->node_pt); // quad point 2
		quad_corner_pts.push_back(quad.nd3->node_pt); // quad point 3
		quad_corner_pts.push_back(quad.nd4->node_pt); // quad point 4

		// Quad mid point
		glm::vec3 quad_midpt = geom_parameters::findGeometricCenter(quad_corner_pts);

		quad_midnode[quad.quad_id].mid_pt = quad_midpt;
	}

	stopwatch_elapsed_str.str("");
	stopwatch_elapsed_str << stopwatch.elapsed();
	std::cout << "Node maping completed at " << stopwatch_elapsed_str.str() << " secs" << std::endl;

	//________________________________________________________________________________________________________________
	int num_intervals = this->matrix_size;
	double progress_interval = num_intervals / 10.0;
	int last_printed_progress = -1; // Initialize with an invalid value

	for (int i =0; i<this->matrix_size; i++)
	{

		double t_eigen = (eigen_freq[i].root_value / c_radius) * c_param;

		// Angular frequency wn
		angular_freq_vector.coeffRef(i) = t_eigen;

		// Eigen value
		eigen_values_vector.coeffRef(i) = (t_eigen * t_eigen);

		this->number_of_modes++;

		// Frequency
		double nat_freq = t_eigen / (2.0 * m_pi);

		if (this->number_of_modes <= paint_mode_count)
		{
			// Modal results
			std::stringstream ss;
			ss << std::fixed << std::setprecision(3) << nat_freq;

			// Add to the string list
			mode_result_str.push_back("Mode " + std::to_string(i + 1) + " = " + ss.str() + " Hz (m ="
				+ std::to_string(eigen_freq[i].m) + ", n=" + std::to_string(eigen_freq[i].n) + ")");
		}

		// Node
		for (auto& nd_m : model_nodes.nodeMap)
		{
			// Get the node ID
			node_id = nd_m.first;

			if (constrained_node_map[node_id] == false)
			{
				// Node is not constrained, so get the co-ordinate
				glm::vec3 node_pt = nd_m.second.node_pt;

				int matrix_index = nodeid_map[node_id];

				// Eigen vectors
				double t_eigen_vec = bessel_eigen_vec(eigen_freq[i], node_pt, c_radius);

				// Add the eigen vectors
				eigen_vectors_matrix.coeffRef(matrix_index, i) = t_eigen_vec;
			}
		}

		// Normalize the eigen vector matrix
		double column_max = eigen_vectors_matrix.col(i).maxCoeff();

		eigen_vectors_matrix.col(i) = eigen_vectors_matrix.col(i) / column_max;

		// Quad
		// Quad Eigen vector calculation
		if (this->number_of_modes <= paint_mode_count)
		{
			for (auto& quad_m : model_quadelements.elementquadMap)
			{
				elementquad_store quad = quad_m.second;

				int quad_id = quad.quad_id;

				//_________________________________________________________________________________________________
				double t_eigen_vec = bessel_eigen_vec(eigen_freq[i], quad_midnode[quad_id].mid_pt, c_radius) / column_max;

				glm::vec3 quad_midpt_eigvec = glm::vec3(0.0, 0.0,t_eigen_vec); // eigen vector at quad mid pt
				
				// Add to the quad mid node eigenvector list
				quad_midnode[quad_id].midpt_displ.push_back(quad_midpt_eigvec);
				quad_midnode[quad_id].midpt_displ_mag.push_back(std::abs(t_eigen_vec));

			}
		}

		// Calculate percentage progress
		int progress_percentage = static_cast<int>((i / static_cast<float>(this->matrix_size)) * 100);
		// Check if it's a new 10% interval
		if ((progress_percentage / 10) > last_printed_progress)
		{
			stopwatch_elapsed_str.str("");
			stopwatch_elapsed_str << stopwatch.elapsed();
			std::cout << progress_percentage << "% modal analysis completed at " << stopwatch_elapsed_str.str() << " secs" << std::endl;
			last_printed_progress = (progress_percentage / 10);
		}
	}


	//_____________________________________________________________________________________
	// Map the results

	for (auto& nd_m : model_nodes.nodeMap)
	{
		int node_id = nd_m.first;
		glm::vec3 node_pt = nd_m.second.node_pt;

		// Modal analysis results
		std::vector<glm::vec3> node_modal_displ;
		std::vector<double> node_modal_displ_magnitude;

		// Check whether the node is fixed or not
		if (this->constrained_node_map[node_id] == false)
		{
			int matrix_index = nodeid_map[node_id];

			for (int i = 0; i < paint_mode_count; i++)
			{
				double displ_magnitude = static_cast<float>(eigen_vectors_matrix.coeff(matrix_index, i));
				
				// get the appropriate modal displacement of this particular node
				glm::vec3 modal_displ = glm::vec3(0.0, 0.0, displ_magnitude);

				// add to modal result of this node
				node_modal_displ.push_back(modal_displ);
				node_modal_displ_magnitude.push_back(std::abs(displ_magnitude));
			}
		}
		else
		{
			// Node is constrained so make it zero
			node_modal_displ.resize(paint_mode_count, glm::vec3(0.0f));
			node_modal_displ_magnitude.resize(paint_mode_count, 0.0);
		}

		// Create the modal analysis result node
		modal_result_nodes.add_result_node(node_id, node_pt, node_modal_displ, node_modal_displ_magnitude);
	}

	stopwatch_elapsed_str.str("");
	stopwatch_elapsed_str << stopwatch.elapsed();
	std::cout << "Results mapped to model nodes at " << stopwatch_elapsed_str.str() << " secs" << std::endl;
	//____________________________________________________________________________________________________________________

	// Add the modal line element result
	for (auto& ln_m : model_lineelements.elementlineMap)
	{
		elementline_store ln = ln_m.second;

		modal_result_lineelements.add_modal_elementline(ln.line_id,
			&modal_result_nodes.modal_nodeMap[ln.startNode->node_id],
			&modal_result_nodes.modal_nodeMap[ln.endNode->node_id]);
	}


	stopwatch_elapsed_str.str("");
	stopwatch_elapsed_str << stopwatch.elapsed();
	std::cout << "Results mapped to model Line Elements at " << stopwatch_elapsed_str.str() << " secs" << std::endl;
	//____________________________________________________________________________________________________________________

	// Add the modal tri element result
	for (auto& tri_m : model_trielements.elementtriMap)
	{
		elementtri_store tri = tri_m.second;
		int tri_id = tri.tri_id;

		modal_result_trielements.add_modal_elementtriangle(tri_id,
			&modal_result_nodes.modal_nodeMap[tri.nd1->node_id],
			&modal_result_nodes.modal_nodeMap[tri.nd2->node_id],
			&modal_result_nodes.modal_nodeMap[tri.nd3->node_id]);
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

		modal_result_quadelements.add_modal_elementquadrilateral(quad_id,
			&modal_result_nodes.modal_nodeMap[quad.nd1->node_id],
			&modal_result_nodes.modal_nodeMap[quad.nd2->node_id],
			&modal_result_nodes.modal_nodeMap[quad.nd3->node_id],
			&modal_result_nodes.modal_nodeMap[quad.nd4->node_id],
			quad_midnode[quad_id].mid_pt,
			quad_midnode[quad_id].midpt_displ,
			quad_midnode[quad_id].midpt_displ_mag);
	}


	stopwatch_elapsed_str.str("");
	stopwatch_elapsed_str << stopwatch.elapsed();
	std::cout << "Results mapped to model Quad Elements at " << stopwatch_elapsed_str.str() << " secs" << std::endl;
	//____________________________________________________________________________________________________________________



}


double modal_analysis_solver::bessel_eigen_vec(const bessel_function_Frequency& bessel_root_i, const glm::vec3& nodept, const double& c_radius)
{
	double nd_radius = glm::length(nodept); // get the radius (origin is 0,0)
	double nd_theta = std::atan2(nodept.y, nodept.x); // get the theta
	double radius_ratio = nd_radius / c_radius; // Radius ratio
	double t_eigen_vec = 0.0;

	// Eigen vector at the node point
	if (nd_radius < (c_radius - 0.1))
	{
		t_eigen_vec = std::cyl_bessel_j(bessel_root_i.m, bessel_root_i.root_value * radius_ratio) * std::cos(bessel_root_i.m *( nd_theta + bessel_root_i.phase_pi));

	}

	return t_eigen_vec;
}


void modal_analysis_solver::modal_analysis_model_rectangular(const nodes_list_store& model_nodes,
	const elementline_list_store& model_lineelements,
	const elementquad_list_store& model_quadelements,
	const material_data& mat_data,
	const double& length_x,
	const double& length_y,
	modal_nodes_list_store& modal_result_nodes,
	modal_elementline_list_store& modal_result_lineelements,
	modal_elementquad_list_store& modal_result_quadelements)
{
	// Rectangular membrane
	int node_id = 0;
	double c_param = std::sqrt(mat_data.line_tension / mat_data.material_density);
	double length_x_squared = length_x * length_x;
	double length_y_squared = length_y * length_y;

	this->number_of_modes = 0;

	// Find the modal frequency
	int modal_m_count = std::ceilf(std::sqrt(this->matrix_size));

	// Eigen function Frequency (Not Bessel!!! for Rectangular membrane)
	std::vector<bessel_function_Frequency> t_rect_freq;

	for (int m = 1; m < modal_m_count+1; m++)
	{
		for (int n = 1; n < modal_m_count+1; n++)
		{
			double ang_freq = glm::pi<float>()*std::sqrt(((m*m)/ length_x_squared) + ((n * n) / length_y_squared));

			// Temp 
			bessel_function_Frequency temp_rect_freq;
			temp_rect_freq.m = m;
			temp_rect_freq.n = n;
			temp_rect_freq.root_value = ang_freq;

			// Add to the list
			t_rect_freq.push_back(temp_rect_freq);
		}
	}

	// Sort the vector based on root_value
	std::sort(t_rect_freq.begin(), t_rect_freq.end(), compareRootValues);

	// Final Eigen function Frequency
	eigen_freq.clear();
	int temp_mode_number = 0;

	// std::ofstream outFile("bessel_roots.txt"); // Open a file for writing

	for (auto& t_freq : t_rect_freq)
	{
		// Temp 
		bessel_function_Frequency temp_rect_freq;
		temp_rect_freq.mode_number = temp_mode_number;
		temp_rect_freq.m = t_freq.m;
		temp_rect_freq.n = t_freq.n;
		temp_rect_freq.root_value = t_freq.root_value;

		// Add to the list
		eigen_freq.push_back(temp_rect_freq);

		// outFile << temp_mode_number << ", " << b_root.m << ", "<< b_root.n << ", "<<b_root.root_value << std::endl; // Write to the file

		temp_mode_number++;
	}

	// outFile.close(); // Close the file

	// Create the eigen vector node map
	int j = 0;
	this->constrained_node_map.clear();
	this->nodeid_map.clear();

	for (auto& nd_m : model_nodes.nodeMap)
	{
		node_id = nd_m.first;
		glm::vec3 node_pt = nd_m.second.node_pt;
		
		if (node_pt.x <= 0.1 || node_pt.y <= 0.1 || node_pt.x >= (length_x -0.1) || node_pt.y >= (length_y - 0.1))
		{
			// Zero for fixed nodes
			this->constrained_node_map[node_id] = true; // is constrained
			continue;
		}

		this->constrained_node_map[node_id] = false; // Node is not fixed
		this->nodeid_map[node_id] = j;
		j++;
	}

	//________________________________________________________________________________________________________________

	// Create the mode shapes
	this->number_of_modes = 0;

	std::unordered_map<int, quad_midnode_eigenvector_store> quad_midnode;

	// Create the quad mid nodes
	for (auto& quad_m : model_quadelements.elementquadMap)
	{
		elementquad_store quad = quad_m.second;

		// Get the mid point of the quad
		std::vector<glm::vec3> quad_corner_pts;

		quad_corner_pts.push_back(quad.nd1->node_pt); // quad point 1
		quad_corner_pts.push_back(quad.nd2->node_pt); // quad point 2
		quad_corner_pts.push_back(quad.nd3->node_pt); // quad point 3
		quad_corner_pts.push_back(quad.nd4->node_pt); // quad point 4

		// Quad mid point
		glm::vec3 quad_midpt = geom_parameters::findGeometricCenter(quad_corner_pts);

		quad_midnode[quad.quad_id].mid_pt = quad_midpt;
	}

	stopwatch_elapsed_str.str("");
	stopwatch_elapsed_str << stopwatch.elapsed();
	std::cout << "Node maping completed at " << stopwatch_elapsed_str.str() << " secs" << std::endl;
	//________________________________________________________________________________________________________________

	for (int i = 0; i < this->matrix_size; i++)
	{

		double t_eigen = eigen_freq[i].root_value * c_param;

		// Angular frequency wn
		angular_freq_vector.coeffRef(i) = t_eigen;

		// Eigen value
		eigen_values_vector.coeffRef(i) = (t_eigen * t_eigen);

		this->number_of_modes++;

		// Frequency
		double nat_freq = t_eigen / (2.0 * m_pi);

		if (this->number_of_modes <= paint_mode_count)
		{
			// Modal results
			std::stringstream ss;
			ss << std::fixed << std::setprecision(3) << nat_freq;

			// Add to the string list
			mode_result_str.push_back("Mode " + std::to_string(i + 1) + " = " + ss.str() + " Hz (m ="
				+ std::to_string(eigen_freq[i].m) + ", n=" + std::to_string(eigen_freq[i].n) + ")");
		}

		// Node
		for (auto& nd_m : model_nodes.nodeMap)
		{
			// Get the node ID
			node_id = nd_m.first;

			if (this->constrained_node_map[node_id] == false)
			{
				// Node is not constrained, so get the co-ordinate
				glm::vec3 node_pt = nd_m.second.node_pt;

				int matrix_index = nodeid_map[node_id];

				// Eigen vectors
				double t_eigen_vec = rect_eigen_vec(eigen_freq[i], node_pt, length_x, length_y);

				// Add the eigen vectors
				eigen_vectors_matrix.coeffRef(matrix_index, i) = t_eigen_vec;
			}

		}

		// Normalize the eigen vector matrix
		double column_max = eigen_vectors_matrix.col(i).maxCoeff();

		eigen_vectors_matrix.col(i) = eigen_vectors_matrix.col(i) / column_max;

		// Quad
		// Quad Eigen vector calculation
		if (this->number_of_modes <= paint_mode_count)
		{
			for (auto& quad_m : model_quadelements.elementquadMap)
			{
				elementquad_store quad = quad_m.second;

				int quad_id = quad.quad_id;

				//_________________________________________________________________________________________________
				double t_eigen_vec = rect_eigen_vec(eigen_freq[i], quad_midnode[quad_id].mid_pt, length_x, length_y) / column_max;

				glm::vec3 quad_midpt_eigvec = glm::vec3(0.0, 0.0,t_eigen_vec); // eigen vector at quad mid pt

				// Add to the quad mid node eigenvector list
				quad_midnode[quad_id].midpt_displ.push_back(quad_midpt_eigvec);
				quad_midnode[quad_id].midpt_displ_mag.push_back(std::abs(t_eigen_vec));

			}
		}
	}

	//_____________________________________________________________________________________
	// Map the results

	for (auto& nd_m : model_nodes.nodeMap)
	{
		int node_id = nd_m.first;
		glm::vec3 node_pt = nd_m.second.node_pt;
		// Modal analysis results
		std::vector<glm::vec3> node_modal_displ;
		std::vector<double> node_modal_displ_magnitude;

		// Check whether the node is fixed or not
		if (this->constrained_node_map[node_id] == false)
		{
			int matrix_index = nodeid_map[node_id];

			for (int i = 0; i < paint_mode_count; i++)
			{
				double displ_magnitude = static_cast<float>(eigen_vectors_matrix.coeff(matrix_index, i));
				// get the appropriate modal displacement of this particular node
				glm::vec3 modal_displ = glm::vec3(0.0, 0.0, displ_magnitude);

				// add to modal result of this node
				node_modal_displ.push_back(modal_displ);
				node_modal_displ_magnitude.push_back(std::abs(displ_magnitude));
			}
		}
		else
		{
			// Node is constrained so make it zero
			node_modal_displ.resize(paint_mode_count, glm::vec3(0.0f));
			node_modal_displ_magnitude.resize(paint_mode_count, 0.0);
		}

		// Create the modal analysis result node
		modal_result_nodes.add_result_node(node_id, node_pt, node_modal_displ, node_modal_displ_magnitude);
	}

	stopwatch_elapsed_str.str("");
	stopwatch_elapsed_str << stopwatch.elapsed();
	std::cout << "Results mapped to model nodes at " << stopwatch_elapsed_str.str() << " secs" << std::endl;
	//____________________________________________________________________________________________________________________

	// Add the modal line element result
	for (auto& ln_m : model_lineelements.elementlineMap)
	{
		elementline_store ln = ln_m.second;

		modal_result_lineelements.add_modal_elementline(ln.line_id,
			&modal_result_nodes.modal_nodeMap[ln.startNode->node_id],
			&modal_result_nodes.modal_nodeMap[ln.endNode->node_id]);
	}


	stopwatch_elapsed_str.str("");
	stopwatch_elapsed_str << stopwatch.elapsed();
	std::cout << "Results mapped to model Line Elements at " << stopwatch_elapsed_str.str() << " secs" << std::endl;
	//____________________________________________________________________________________________________________________

	// Add the modal quad element result
	for (auto& quad_m : model_quadelements.elementquadMap)
	{
		elementquad_store quad = quad_m.second;
		int quad_id = quad.quad_id;

		modal_result_quadelements.add_modal_elementquadrilateral(quad_id,
			&modal_result_nodes.modal_nodeMap[quad.nd1->node_id],
			&modal_result_nodes.modal_nodeMap[quad.nd2->node_id],
			&modal_result_nodes.modal_nodeMap[quad.nd3->node_id],
			&modal_result_nodes.modal_nodeMap[quad.nd4->node_id],
			quad_midnode[quad_id].mid_pt,
			quad_midnode[quad_id].midpt_displ,
			quad_midnode[quad_id].midpt_displ_mag);
	}


	stopwatch_elapsed_str.str("");
	stopwatch_elapsed_str << stopwatch.elapsed();
	std::cout << "Results mapped to model Quad Elements at " << stopwatch_elapsed_str.str() << " secs" << std::endl;
	//____________________________________________________________________________________________________________________

}


double modal_analysis_solver::rect_eigen_vec(const bessel_function_Frequency& rect_freq_i, const glm::vec3& nodept, const double& length_x, const double& length_y)
{
	double nd_x = nodept.x; // Node pt x
	double nd_y = nodept.y; // Node pt y
	
	double t_eigen_vec = 0.0;

	// Eigen vector at the node point
	if (nd_x > 0.1 || nd_y > 0.1 || nd_x < (length_x - 0.1) || nd_y < (length_y - 0.1))
	{
		t_eigen_vec = std::sin((glm::pi<float>() * rect_freq_i.m * nd_x)/length_x) * std::sin((glm::pi<float>() * rect_freq_i.n * nd_y) / length_y);

	}

	return t_eigen_vec;

}

