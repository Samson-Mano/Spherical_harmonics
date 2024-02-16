#pragma once
#include <iostream>
#include <fstream>

// FE Objects
#include "../geometry_store/fe_objects/nodes_list_store.h"
#include "../geometry_store/fe_objects/elementline_list_store.h"
#include "../geometry_store/fe_objects/elementtri_list_store.h"
#include "../geometry_store/fe_objects/elementquad_list_store.h"

// FE Results Modal Analysis
#include "../geometry_store/result_objects/modal_nodes_list_store.h"
#include "../geometry_store/result_objects/modal_elementline_list_store.h"
#include "../geometry_store/result_objects/modal_elementtri_list_store.h"
#include "../geometry_store/result_objects/modal_elementquad_list_store.h"

// Stop watch
#include "../events_handler/Stopwatch_events.h"

#include <cmath>
#include <boost/math/special_functions/bessel.hpp>

#pragma warning(push)
#pragma warning (disable : 26451)
#pragma warning (disable : 26495)
#pragma warning (disable : 6255)
#pragma warning (disable : 6294)
#pragma warning (disable : 26813)
#pragma warning (disable : 26454)

// Optimization for Eigen Library
// 1) OpenMP (Yes (/openmp)
//	 Solution Explorer->Configuration Properties -> C/C++ -> Language -> Open MP Support
// 2) For -march=native, choose "AVX2" or the latest supported instruction set.
//   Solution Explorer->Configuration Properties -> C/C++ -> Code Generation -> Enable Enhanced Instruction Set 

#include <Eigen/Dense>
#include <Eigen/Sparse>
#include <Eigen/SparseLU>
#include <Eigen/Eigenvalues>
// Define the sparse matrix type for the reduced global stiffness matrix
typedef Eigen::SparseMatrix<double> SparseMatrix;
#pragma warning(pop)

typedef boost::math::policies::policy<
	boost::math::policies::domain_error<boost::math::policies::ignore_error>,
	boost::math::policies::overflow_error<boost::math::policies::ignore_error>,
	boost::math::policies::underflow_error<boost::math::policies::ignore_error>,
	boost::math::policies::denorm_error<boost::math::policies::ignore_error>,
	boost::math::policies::pole_error<boost::math::policies::ignore_error>,
	boost::math::policies::evaluation_error<boost::math::policies::ignore_error>
> ignore_all_policy;

struct quad_midnode_eigenvector_store
{
	int quad_id = 0;
	glm::vec3 mid_pt = glm::vec3(0);
	std::vector<glm::vec3> midpt_displ; // eigen vector at mid pt of quad
	std::vector<double> midpt_displ_mag; // eigen vector magnitude at mid pt of quad
};


struct bessel_function_Frequency
{
	int mode_number = 0; // mode number
	int m = 0; // Bessel function m order
	int n = 0; // n th root
	double root_value = 0.0; // Bessel function root value
	double phase_pi = 0.0; // Used only for circular membrane (Not used for rectangular membrane)
};

// Comparator function for sorting based on root_value
inline bool compareRootValues(const bessel_function_Frequency& a, const bessel_function_Frequency& b) 
{
	return a.root_value < b.root_value;
}

class modal_analysis_solver
{
public:
	// Result store
	std::unordered_map<int, bool> constrained_node_map; // Node ID and Bool True = Constrained, False = Un Constrained
	std::unordered_map<int, int> nodeid_map; // Node ID map for eigen vectors
	int paint_mode_count = 100; // Draw only first 100 modes (To save memory)
	int number_of_modes = 0;
	int node_count = 0;
	int matrix_size = 0;
	int model_type = 0;

	// Eigen values matrices
	Eigen::VectorXd angular_freq_vector;
	Eigen::VectorXd eigen_values_vector;

	// Eigen vector matrices
	// Eigen::MatrixXd displ_vectors_matrix;
	Eigen::MatrixXd eigen_vectors_matrix;

	std::vector<std::string> mode_result_str;
	bool is_modal_analysis_complete = false;

	modal_analysis_solver();
	~modal_analysis_solver();
	void clear_results();

	void modal_analysis_start(const nodes_list_store& model_nodes,
		const elementline_list_store& model_lineelements,
		const elementtri_list_store& model_trielements,
		const elementquad_list_store& model_quadelements,
		const material_data& mat_data,
		modal_nodes_list_store& modal_result_nodes,
		modal_elementline_list_store& modal_result_lineelements,
		modal_elementtri_list_store& modal_result_trielements,
		modal_elementquad_list_store& modal_result_quadelements);
private:
	const double m_pi = 3.14159265358979323846;
	const double epsilon = 0.000001;
	// const bool print_matrix = true;
	Stopwatch_events stopwatch;
	std::stringstream stopwatch_elapsed_str;

	std::vector<bessel_function_Frequency> eigen_freq;

	void modal_analysis_model_circular(const nodes_list_store& model_nodes,
		const elementline_list_store& model_lineelements,
		const elementtri_list_store& model_trielements,
		const elementquad_list_store& model_quadelements,
		const material_data& mat_data,
		const double& c_radius,
		modal_nodes_list_store& modal_result_nodes,
		modal_elementline_list_store& modal_result_lineelements,
		modal_elementtri_list_store& modal_result_trielements,
		modal_elementquad_list_store& modal_result_quadelements);

	double bessel_eigen_vec(const bessel_function_Frequency& bessel_root_i, const glm::vec3& nodept, const double& c_radius);

	void modal_analysis_model_rectangular(const nodes_list_store& model_nodes,
		const elementline_list_store& model_lineelements,
		const elementquad_list_store& model_quadelements,
		const material_data& mat_data,
		const double& length_x,
		const double& length_y,
		modal_nodes_list_store& modal_result_nodes,
		modal_elementline_list_store& modal_result_lineelements,
		modal_elementquad_list_store& modal_result_quadelements);

	double rect_eigen_vec(const bessel_function_Frequency& rect_freq_i, const glm::vec3& nodept, const double& length_x, const double& length_y);

};
