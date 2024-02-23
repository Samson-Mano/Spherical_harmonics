#include "pulse_analysis_solver.h"

pulse_analysis_solver::pulse_analysis_solver()
{
	// Empty constructor
}

pulse_analysis_solver::~pulse_analysis_solver()
{
	// Empty destructor
}

void pulse_analysis_solver::clear_results()
{
	// Clear the analysis results
	is_pulse_analysis_complete = false;
	time_step_count = 0;
	time_interval = 0.0;
	total_simulation_time = 0.0;

}

void pulse_analysis_solver::pulse_analysis_start(const nodes_list_store& model_nodes,
	const elementtri_list_store& model_trielements,
	const elementquad_list_store& model_quadelements,
	const nodeload_list_store& node_loads,
	const nodeinlcond_list_store& node_inldispl,
	const nodeinlcond_list_store& node_inlvelo,
	const material_data& mat_data,
	const modal_analysis_solver& modal_solver,
	const double total_simulation_time,
	const double time_interval,
	const double damping_ratio,
	const int selected_pulse_option,
	rslt_nodes_list_store& pulse_result_nodes,
	rslt_elementtri_list_store& pulse_result_trielements,
	rslt_elementquad_list_store& pulse_result_quadelements)
{
	// Main solver call
	this->is_pulse_analysis_complete = false;

	// Check the model
	// Number of loads, initial condition (Exit if no load and no initial condition is present)
	if (node_loads.load_count == 0 &&
		static_cast<int>(node_inldispl.inlcondMap.size()) == 0 &&
		static_cast<int>(node_inlvelo.inlcondMap.size()) == 0)
	{
		return;
	}

	//____________________________________________
	Eigen::initParallel();  // Initialize Eigen's thread pool

	stopwatch.start();

	stopwatch_elapsed_str.str("");
	stopwatch_elapsed_str << std::fixed << std::setprecision(6);
	std::cout << "Pulse analysis started" << std::endl;

	// Retrive the Eigen values and Eigen vectors data from Modan Analysis solver
	this->nodeid_map = modal_solver.nodeid_map;
	this->model_type = modal_solver.model_type;
	this->numDOF = modal_solver.node_count;
	this->reducedDOF = modal_solver.matrix_size;
	this->eigen_values_vector = modal_solver.eigen_values_vector;
	this->eigen_vectors_matrix = modal_solver.eigen_vectors_matrix;

	//std::ofstream output_file;
	//output_file.open("pulse_analysis_results.txt");

	//output_file << "Eigen vector matrix:" << std::endl;
	//output_file << this->eigen_vectors_matrix << std::endl;
	//output_file << std::endl;

	//output_file.close();


	//____________________________________________________________________________________________________________________
	// Step: 1 Create the initial displacement matrix (Modal Transformed initial displacement matrix)
	Eigen::VectorXd modal_reducedInitialDisplacementMatrix(reducedDOF);
	Eigen::VectorXd modal_reducedInitialVelocityMatrix(reducedDOF);


	create_initial_condition_matrices(modal_reducedInitialDisplacementMatrix,
		modal_reducedInitialVelocityMatrix,
		model_nodes,
		node_inldispl,
		node_inlvelo);


	stopwatch_elapsed_str.str("");
	stopwatch_elapsed_str << stopwatch.elapsed();
	std::cout << "Intial condition matrices created at " << stopwatch_elapsed_str.str() << " secs" << std::endl;

	//____________________________________________________________________________________________________________________
	// Step: 2 Create the pulse load matrix (Modal Transformed pulse loads)

	this->pulse_loads.clear();

	for (auto& ld_m : node_loads.loadMap)
	{
		// get the loads
		load_data ld = ld_m.second;

		// temporary value to store the pulse load
		pulse_load_data pulse_ld;

		create_pulse_load_matrices(pulse_ld,
			ld,
			model_nodes);

		// Set the time data to the pulse loads
		pulse_ld.node_id = ld.node_id;
		pulse_ld.load_start_time = ld.load_start_time;
		pulse_ld.load_end_time = ld.load_end_time;

		// Add to the pulse load list
		this->pulse_loads.push_back(pulse_ld);
	}


	stopwatch_elapsed_str.str("");
	stopwatch_elapsed_str << stopwatch.elapsed();
	std::cout << "Pulse Load matrices created at " << stopwatch_elapsed_str.str() << " secs" << std::endl;

	//____________________________________________________________________________________________________________________
	// Step: 3 Find the pulse response
	std::unordered_map<int, pulse_node_result> node_results;

	// Time step count
	this->time_step_count = 0;

	int num_intervals = static_cast<int>(total_simulation_time / time_interval);
	double progress_interval = num_intervals / 10.0;
	int last_printed_progress = -1; // Initialize with an invalid value

	for (int i = 0; i <= num_intervals; i++)
	{

		// time at 
		double time_t = i * time_interval;

		// Displacement amplitude matrix
		Eigen::VectorXd modal_displ_ampl_respMatrix(reducedDOF);
		modal_displ_ampl_respMatrix.setZero();

		// 1D results for modal transformed Simple Harmonic Motion
		for (int i = 0; i < reducedDOF; i++)
		{
			double modal_mass = 1.0;
			double modal_stiff = eigen_values_vector.coeff(i);

			// Displacement response due to initial condition
			double displ_resp_initial = 0.0;

			// Get the steady state displacemetn response for the initial condition
			if (std::abs(modal_reducedInitialDisplacementMatrix.coeff(i)) > epsilon ||
				std::abs(modal_reducedInitialVelocityMatrix.coeff(i)) > epsilon)
			{
				displ_resp_initial = get_steady_state_initial_condition_soln(time_t,
					modal_mass,
					modal_stiff,
					modal_reducedInitialDisplacementMatrix.coeff(i),
					modal_reducedInitialVelocityMatrix.coeff(i));
			}

			//_______________________________________________________________________
			// Displacement response due to pulse force
			double displ_resp_force = 0.0;

			// Cycle through all the loads
			for (auto& pulse_load : pulse_loads)
			{
				if (std::abs(pulse_load.modal_LoadamplMatrix(i)) > epsilon)
				{
					// Load amplitude at index not equal to zero
					// Go through all the force
					double at_force_displ_resp = 0.0;

					if (selected_pulse_option == 0)
					{
						// Half sine pulse

						at_force_displ_resp = get_steady_state_half_sine_pulse_soln(time_t,
							modal_mass,
							modal_stiff,
							pulse_load.modal_LoadamplMatrix(i),
							pulse_load.load_start_time,
							pulse_load.load_end_time);

					}
					else if (selected_pulse_option == 1)
					{
						// Rectangular pulse

						at_force_displ_resp = get_steady_state_rectangular_pulse_soln(time_t,
							modal_mass,
							modal_stiff,
							pulse_load.modal_LoadamplMatrix(i),
							pulse_load.load_start_time,
							pulse_load.load_end_time);

					}
					else if (selected_pulse_option == 2)
					{
						// Triangular pulse

						at_force_displ_resp = get_steady_state_triangular_pulse_soln(time_t,
							modal_mass,
							modal_stiff,
							pulse_load.modal_LoadamplMatrix(i),
							pulse_load.load_start_time,
							pulse_load.load_end_time);

					}
					else if (selected_pulse_option == 3)
					{
						// Step force with finite rise

						at_force_displ_resp = get_steady_state_stepforce_finiterise_soln(time_t,
							modal_mass,
							modal_stiff,
							pulse_load.modal_LoadamplMatrix(i),
							pulse_load.load_start_time,
							pulse_load.load_end_time);

					}
					else if (selected_pulse_option == 4)
					{
						// Harmonic Excitation

						at_force_displ_resp = get_total_harmonic_soln(time_t,
							modal_mass,
							modal_stiff,
							pulse_load.modal_LoadamplMatrix(i),
							pulse_load.load_start_time,
							pulse_load.load_end_time);

					}

					displ_resp_force = displ_resp_force + at_force_displ_resp;
				}

			}

			// Store the displacement result
			modal_displ_ampl_respMatrix.coeffRef(i) = displ_resp_initial + displ_resp_force;


			// Calculate percentage progress
			int progress_percentage = static_cast<int>((time_t / total_simulation_time) * 100);
			// Check if it's a new 10% interval
			if (progress_percentage / 10 > last_printed_progress)
			{
				stopwatch_elapsed_str.str("");
				stopwatch_elapsed_str << stopwatch.elapsed();
				std::cout << progress_percentage << "% pulse analysis completed at " << stopwatch_elapsed_str.str() << " secs" << std::endl;
				last_printed_progress = progress_percentage / 10;
			}
		}


		// Transform the modal displacement to the global displacement
		Eigen::VectorXd global_displ_ampl_respMatrix(reducedDOF);

		global_displ_ampl_respMatrix = this->eigen_vectors_matrix * modal_displ_ampl_respMatrix;

		// Store the results to node results
		for (auto& nd_m : model_nodes.nodeMap)
		{
			// get the node id
			int node_id = nd_m.second.node_id;

			// Node displacement response
			glm::vec3 node_displ = glm::vec3(0);
			double displ_magnitude = 0.0;

			// Check whether the node is fixed or not

			int matrix_index = nodeid_map[node_id];

			// get the nodal displacement at time t
			node_displ = glm::vec3(0.0, 0.0, global_displ_ampl_respMatrix.coeff(matrix_index));
			displ_magnitude = std::abs(global_displ_ampl_respMatrix.coeff(matrix_index));


			// Add the index
			node_results[node_id].at_time_step.push_back(this->time_step_count);
			// Add the time val
			node_results[node_id].time_val.push_back(time_t);
			// Add the displacement magnitude
			node_results[node_id].node_displ_magnitude.push_back(displ_magnitude);
			// Add the Normalized displacement
			node_results[node_id].node_displ.push_back(node_displ);
		}

		// iterate the time step count
		this->time_step_count++;
	}

	stopwatch_elapsed_str.str("");
	stopwatch_elapsed_str << stopwatch.elapsed();
	std::cout << "Pulse analysis solved at " << stopwatch_elapsed_str.str() << " secs" << std::endl;

	//____________________________________________________________________________________________________________________
	// Step: 4 Map the results

	map_pulse_analysis_results(pulse_result_nodes,
		pulse_result_trielements,
		pulse_result_quadelements,
		this->time_step_count,
		model_nodes,
		model_trielements,
		model_quadelements,
		node_results);



	stopwatch_elapsed_str.str("");
	stopwatch_elapsed_str << stopwatch.elapsed();
	std::cout << "Results mapped at " << stopwatch_elapsed_str.str() << " secs" << std::endl;

	//____________________________________________________________________________________________________________________
	// Save the Pulse analysis settings
	this->time_interval = time_interval;
	this->total_simulation_time = total_simulation_time;

	if (pulse_result_nodes.rslt_maxdispl == 0)
	{
		// Analysis failed 
		stopwatch_elapsed_str.str("");
		stopwatch_elapsed_str << stopwatch.elapsed();
		std::cout << "Analysis failed at " << stopwatch_elapsed_str.str() << " secs" << std::endl;
		return;
	}

	stopwatch_elapsed_str.str("");
	stopwatch_elapsed_str.clear();
	stopwatch_elapsed_str << stopwatch.elapsed();
	std::cout << "Results storage completed at " << stopwatch_elapsed_str.str() << " secs" << std::endl;

	// Analysis complete
	this->is_pulse_analysis_complete = true;

}

void pulse_analysis_solver::create_initial_condition_matrices(Eigen::VectorXd& modal_reducedInitialDisplacementMatrix,
	Eigen::VectorXd& modal_reducedInitialVelocityMatrix,
	const nodes_list_store& model_nodes,
	const nodeinlcond_list_store& node_inldispl,
	const nodeinlcond_list_store& node_inlvelo)
{
	// Modal reduction of global initial displacement
	// Modal vector transformation 
	modal_reducedInitialDisplacementMatrix.setZero();
	modal_reducedInitialVelocityMatrix.setZero();

	// Fixed  - Fixed Condition so skip the first and last node
	Eigen::VectorXd global_reducedInitialDisplacementMatrix(reducedDOF);
	Eigen::VectorXd global_reducedInitialVelocityMatrix(reducedDOF);

	// Set the initial condition - Displacement
	int matrix_index = 0;
	global_reducedInitialDisplacementMatrix.setZero();

	for (auto& inlc_m : node_inldispl.inlcondMap)
	{
		nodeinl_condition_data inlc = inlc_m.second;


		// Node is un-constrained
		// get the matrix index
		matrix_index = this->nodeid_map[inlc.node_id]; // get the ordered map of the start node ID

		global_reducedInitialDisplacementMatrix.coeffRef(matrix_index) = -1.0 * inlc.inl_amplitude_z;

	}

	// Set the initial condition - Velocity
	matrix_index = 0;
	global_reducedInitialVelocityMatrix.setZero();

	for (auto& inlc_m : node_inlvelo.inlcondMap)
	{
		nodeinl_condition_data inlc = inlc_m.second;


		// Node is un-constrained
		// get the matrix index
		matrix_index = this->nodeid_map[inlc.node_id]; // get the ordered map of the start node ID

		global_reducedInitialVelocityMatrix.coeffRef(matrix_index) = -1.0 * inlc.inl_amplitude_z;

	}


	// Apply modal Transformation
	modal_reducedInitialDisplacementMatrix = this->eigen_vectors_matrix.transpose() * global_reducedInitialDisplacementMatrix;
	modal_reducedInitialVelocityMatrix = this->eigen_vectors_matrix.transpose() * global_reducedInitialVelocityMatrix;

}


void pulse_analysis_solver::create_pulse_load_matrices(pulse_load_data& pulse_ld,
	const load_data& ld,
	const nodes_list_store& model_nodes)
{
	// Create Pulse load matrix
	// Modal vector transformation
	int matrix_index = 0;
	Eigen::VectorXd global_reducedLoadMatrix(reducedDOF);
	global_reducedLoadMatrix.setZero();


	// Node is un-constrained
	// get the matrix index
	matrix_index = this->nodeid_map[ld.node_id]; // get the ordered map of the start node ID

	global_reducedLoadMatrix.coeffRef(matrix_index) = ld.load_value;


	// Apply modal Transformation
	Eigen::VectorXd modal_reducedLoadMatrix(reducedDOF);

	modal_reducedLoadMatrix = this->eigen_vectors_matrix.transpose() * global_reducedLoadMatrix;

	// Store the modal amplitude matrix
	pulse_ld.modal_LoadamplMatrix = modal_reducedLoadMatrix;

}


double pulse_analysis_solver::get_steady_state_initial_condition_soln(const double& time_t,
	const double& modal_mass,
	const double& modal_stiff,
	const double& modal_initial_displacement,
	const double& modal_initial_velocity)
{
	// Return the steady state solution for the intial displacment and velocity
	double modal_omega_n = std::sqrt(modal_stiff / modal_mass); // Modal omega n

	double steady_state_displ_resp = (modal_initial_displacement * std::cos(modal_omega_n * time_t)) +
		((modal_initial_velocity / modal_omega_n) * std::sin(modal_omega_n * time_t));

	return steady_state_displ_resp;
}


double pulse_analysis_solver::get_steady_state_half_sine_pulse_soln(const double& time_t,
	const double& modal_mass,
	const double& modal_stiff,
	const double& modal_force_ampl,
	const double& modal_force_starttime,
	const double& modal_force_endtime)
{
	// Return the steady state solution for the half sine pulse
	double modal_omega_n = std::sqrt(modal_stiff / modal_mass); // Modal omega n
	double modal_omega_f = m_pi / (modal_force_endtime - modal_force_starttime);

	// natural time period
	double T_n = (2.0 * m_pi) / modal_omega_n;
	// Force period
	double t_d = (modal_force_endtime - modal_force_starttime);
	// time at
	double t_at = 0.0;

	double steady_state_displ_resp = 0.0;

	// Check whether the current time is greater than the force start time
	if (time_t >= modal_force_starttime)
	{
		t_at = time_t - modal_force_starttime;
		if (time_t <= modal_force_endtime)
		{
			// current time is within the force period
			if (std::abs((t_d / T_n) - 0.5) < 0.000001)
			{
				// Resonance case
				double k_fact = (modal_force_ampl / (2.0 * modal_stiff));
				steady_state_displ_resp = k_fact * (std::sin(modal_omega_n * t_at) - (modal_omega_n * t_at * std::cos(modal_omega_n * t_at)));
			}
			else
			{
				// Normal case
				double const1 = m_pi / (modal_omega_n * t_d);
				double const2 = 1.0 - std::pow(const1, 2);
				double k_fact = (modal_force_ampl / modal_stiff) * (1 / const2);
				steady_state_displ_resp = k_fact * (std::sin((m_pi / t_d) * t_at) - (const1 * std::sin(modal_omega_n * t_at)));
			}
		}
		else if (time_t > modal_force_endtime)
		{
			// current time is over the force period
			if (std::abs((t_d / T_n) - 0.5) < 0.000001)
			{
				// Resonance case
				double k_fact = ((modal_force_ampl * m_pi) / (2.0 * modal_stiff));
				steady_state_displ_resp = k_fact * std::cos((modal_omega_n * t_at) - m_pi);
			}
			else
			{
				// Normal case
				double const1 = m_pi / (modal_omega_n * t_d);
				double const2 = std::pow(const1, 2) - 1.0;
				double k_fact = (modal_force_ampl / modal_stiff) * ((2 * const1) / const2) * std::cos(modal_omega_n * t_d * 0.5);
				steady_state_displ_resp = k_fact * std::sin(modal_omega_n * (t_at - (t_d * 0.5)));
			}
		}
	}


	return steady_state_displ_resp;
}


double pulse_analysis_solver::get_steady_state_rectangular_pulse_soln(const double& time_t,
	const double& modal_mass,
	const double& modal_stiff,
	const double& modal_force_ampl,
	const double& modal_force_starttime,
	const double& modal_force_endtime)
{
	// Return the steady state solution for the Rectangular pulse
	double modal_omega_n = std::sqrt(modal_stiff / modal_mass); // Modal omega n
	double modal_omega_f = m_pi / (modal_force_endtime - modal_force_starttime);

	// natural time period
	double T_n = (2.0 * m_pi) / modal_omega_n;
	// Force period
	double t_d = (modal_force_endtime - modal_force_starttime);
	// time at
	double t_at = 0.0;

	double steady_state_displ_resp = 0.0;

	// Check whether the current time is greater than the force start time
	if (time_t >= modal_force_starttime)
	{
		t_at = time_t - modal_force_starttime;
		double k_fact = (modal_force_ampl / modal_stiff);

		if (time_t <= modal_force_endtime)
		{

			// current time is within the force period
			steady_state_displ_resp = k_fact * (1.0 - std::cos(modal_omega_n * t_at));

		}
		else if (time_t > modal_force_endtime)
		{
			// current time is over the force period
			steady_state_displ_resp = k_fact * (std::cos(modal_omega_n * (t_at - t_d)) - std::cos(modal_omega_n * t_at));

		}
	}

	return steady_state_displ_resp;
}


double pulse_analysis_solver::get_steady_state_triangular_pulse_soln(const double& time_t,
	const double& modal_mass,
	const double& modal_stiff,
	const double& modal_force_ampl,
	const double& modal_force_starttime,
	const double& modal_force_endtime)
{
	// Return the steady state solution for the Triangular pulse
	double modal_omega_n = std::sqrt(modal_stiff / modal_mass); // Modal omega n
	double modal_omega_f = m_pi / (modal_force_endtime - modal_force_starttime);

	// natural time period
	double T_n = (2.0 * m_pi) / modal_omega_n;
	// Force period
	double t_d = (modal_force_endtime - modal_force_starttime);
	// time at
	double t_at = 0.0;

	double	steady_state_displ_resp = 0.0;

	// Check whether the current time is greater than the force start time
	if (time_t >= modal_force_starttime)
	{
		t_at = time_t - modal_force_starttime;

		if (time_t <= modal_force_endtime)
		{
			// current time is within the force period
			if (t_at < (t_d * 0.5))
			{
				double k_fact = ((2.0 * modal_force_ampl) / modal_stiff);

				steady_state_displ_resp = k_fact * ((t_at / t_d) - (std::sin(modal_omega_n * t_at) / (t_d * modal_omega_n)));
			}
			else
			{
				double k_fact = ((2.0 * modal_force_ampl) / modal_stiff);
				double factor1 = (1 / (t_d * modal_omega_n)) * ((2.0 * std::sin(modal_omega_n * (t_at - (0.5 * t_d)))) -
					std::sin(modal_omega_n * t_at));

				steady_state_displ_resp = k_fact * (1.0 - (t_at / t_d) + factor1);
			}

		}
		else if (time_t > modal_force_endtime)
		{
			// current time is over the force period

			double k_fact = ((2.0 * modal_force_ampl) / modal_stiff);
			double m_factor = (1 / (t_d * modal_omega_n));
			double factor1 = 2.0 * std::sin(modal_omega_n * (t_at - (0.5 * t_d)));
			double factor2 = std::sin(modal_omega_n * (t_at - t_d));

			steady_state_displ_resp = k_fact * (m_factor * (factor1 - factor2 - std::sin(modal_omega_n * t_at)));

		}
	}

	return steady_state_displ_resp;
}



double pulse_analysis_solver::get_steady_state_stepforce_finiterise_soln(const double& time_t,
	const double& modal_mass,
	const double& modal_stiff,
	const double& modal_force_ampl,
	const double& modal_force_starttime,
	const double& modal_force_endtime)
{
	// Return the steady state solution for the Step Force with Finite Rise
	double modal_omega_n = std::sqrt(modal_stiff / modal_mass); // Modal omega n
	double modal_omega_f = m_pi / (modal_force_endtime - modal_force_starttime);

	// natural time period
	double T_n = (2.0 * m_pi) / modal_omega_n;
	// Force period
	double t_d = (modal_force_endtime - modal_force_starttime);
	// time at
	double t_at = 0.0;

	double	steady_state_displ_resp = 0.0;

	// Check whether the current time is greater than the force start time
	if (time_t >= modal_force_starttime)
	{
		t_at = time_t - modal_force_starttime;
		double k_fact = (modal_force_ampl / modal_stiff);

		if (time_t <= modal_force_endtime)
		{

			// current time is within the force period
			steady_state_displ_resp = k_fact * ((t_at / t_d) -
				(std::sin(modal_omega_n * t_at) / (t_d * modal_omega_n)));

		}
		else if (time_t > modal_force_endtime)
		{
			// current time is over the force period
			double factor1 = std::sin(modal_omega_n * (t_at - t_d)) - std::sin(modal_omega_n * t_at);

			steady_state_displ_resp = k_fact * (1.0 + (1 / (modal_omega_n * t_d)) * factor1);

		}
	}

	return steady_state_displ_resp;
}


double pulse_analysis_solver::get_total_harmonic_soln(const double& time_t,
	const double& modal_mass,
	const double& modal_stiff,
	const double& modal_force_ampl,
	const double& modal_force_starttime,
	const double& modal_force_endtime)
{
	// Return the Total solution (Transient + steady state) for the Harmonic excitation
		// Return the steady state solution for the Step Force with Finite Rise
	double modal_omega_n = std::sqrt(modal_stiff / modal_mass); // Modal omega n
	double modal_omega_f = m_pi / (2.0 * (modal_force_endtime - modal_force_starttime));

	double transient_displ_resp;
	double steady_state_displ_resp;

	if (std::abs(modal_omega_f - modal_omega_n) < epsilon)
	{
		// Resonance case
		transient_displ_resp = 0.0;

		double force_factor = (modal_force_ampl / (2.0 * modal_stiff));

		steady_state_displ_resp = force_factor * ((modal_omega_n * time_t * std::cos(modal_omega_n * time_t)) - std::sin(modal_omega_n * time_t));
	}
	else
	{
		// Regular case
		double force_factor = (modal_force_ampl / modal_stiff);
		double freq_ratio = modal_omega_f / modal_omega_n;
		double freq_factor = (1.0 - (freq_ratio * freq_ratio));


		transient_displ_resp = -1.0 * force_factor * (freq_ratio / freq_factor) * std::sin(modal_omega_n * time_t);

		steady_state_displ_resp = force_factor * (1.0 / freq_factor) * std::sin(modal_omega_f * time_t);

	}

	return (transient_displ_resp + steady_state_displ_resp);
}

void pulse_analysis_solver::map_pulse_analysis_results(rslt_nodes_list_store& pulse_result_nodes,
	rslt_elementtri_list_store& pulse_result_trielements,
	rslt_elementquad_list_store& pulse_result_quadelements,
	const int& number_of_time_steps,
	const nodes_list_store& model_nodes,
	const elementtri_list_store& model_trielements,
	const elementquad_list_store& model_quadelements,
	const std::unordered_map<int, pulse_node_result>& node_results)
{
	// Map the pulse analysis results
	// map the node results
	pulse_result_nodes.clear_results();

	for (auto& nd_m : model_nodes.nodeMap)
	{
		// Extract the model node
		node_store nd = nd_m.second;

		// Add to the pulse node results store
		pulse_result_nodes.add_result_node(nd.node_id, nd.node_pt(), 
			node_results.at(nd.node_id).node_displ, node_results.at(nd.node_id).node_displ_magnitude);
	}

	//_________________________________________________________________________________________________________________

	double maximum_displacement = 0.0;

	for (auto& nd_m : pulse_result_nodes.rslt_nodeMap)
	{
		rslt_node_store nd = nd_m.second;

		//get all the two points
		// Point displacement
		for (auto& nodept_displ : nd.node_displ_magnitude)
		{
			maximum_displacement = std::max(maximum_displacement, std::abs(nodept_displ));
		}
	}

	//_________________________________________________________________________________________________________________
	// map the tri results
	pulse_result_trielements.clear_results();

	for (auto& tri_m : model_trielements.elementtriMap)
	{
		// Extract the model tris
		elementtri_store tri = tri_m.second;
		int tri_id = tri.tri_id;

		// Extract the pulse node store -> nd1, nd2 & nd3
		rslt_node_store* nd1 = &pulse_result_nodes.rslt_nodeMap[tri.nd1->node_id];
		rslt_node_store* nd2 = &pulse_result_nodes.rslt_nodeMap[tri.nd2->node_id];
		rslt_node_store* nd3 = &pulse_result_nodes.rslt_nodeMap[tri.nd3->node_id];


		// Add to the pulse element tri results store
		pulse_result_trielements.add_rslt_elementtriangle(tri.tri_id, nd1, nd2, nd3);
	}

	//_________________________________________________________________________________________________________________
	// map the quad results
	pulse_result_quadelements.clear_results();

	for (auto& quad_m : model_quadelements.elementquadMap)
	{
		// Extract the model lines
		elementquad_store quad = quad_m.second;
		int quad_id = quad.quad_id;

		// Extract the pulse node store -> nd1, nd2, nd3 & nd4
		rslt_node_store* nd1 = &pulse_result_nodes.rslt_nodeMap[quad.nd1->node_id];
		rslt_node_store* nd2 = &pulse_result_nodes.rslt_nodeMap[quad.nd2->node_id];
		rslt_node_store* nd3 = &pulse_result_nodes.rslt_nodeMap[quad.nd3->node_id];
		rslt_node_store* nd4 = &pulse_result_nodes.rslt_nodeMap[quad.nd4->node_id];


		// Add to the pulse element quad results store
		pulse_result_quadelements.add_rslt_elementquadrilateral(quad.quad_id, nd1, nd2, nd3, nd4);
	}

	//_________________________________________________________________________________________________________________
	// Set the maximim displacement
	pulse_result_nodes.rslt_maxdispl = maximum_displacement;


}