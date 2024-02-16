#include "pulse_elementquad_list_store.h"

pulse_elementquad_list_store::pulse_elementquad_list_store()
{
	// Empty constructor
}

pulse_elementquad_list_store::~pulse_elementquad_list_store()
{
	// Empty destructor
}

void pulse_elementquad_list_store::init(geom_parameters* geom_param_ptr)
{
	// Set the geometry parameters
	this->geom_param_ptr = geom_param_ptr;

	// Set the geometry parameters for the quads (and clear the quads)
	pulse_element_tris1m2.init(geom_param_ptr); // Tri 1m2
	pulse_element_tris2m3.init(geom_param_ptr); // Tri 2m3
	pulse_element_tris3m4.init(geom_param_ptr); // Tri 3m4
	pulse_element_tris4m1.init(geom_param_ptr); // Tri 4m1

	// Clear the element quads
	pulse_elementquad_count = 0;
	pulse_elementquadMap.clear();

}

void pulse_elementquad_list_store::clear_data()
{
	// Clear the element quads
	pulse_element_tris1m2.clear_triangles(); // Tri 1m2
	pulse_element_tris2m3.clear_triangles(); // Tri 2m3
	pulse_element_tris3m4.clear_triangles(); // Tri 3m4
	pulse_element_tris4m1.clear_triangles(); // Tri 4m1

	// Clear the element quads
	pulse_elementquad_count = 0;
	pulse_elementquadMap.clear();

}

void pulse_elementquad_list_store::add_pulse_elementquadrilateral(int& quad_id, pulse_node_store* nd1, pulse_node_store* nd2, 
	pulse_node_store* nd3, 	pulse_node_store* nd4, 
	glm::vec3& midpt, std::vector<glm::vec3>& midpt_displ, std::vector<double>& midpt_displ_mag)
{
	// Add result quad element
	pulse_elementquad_store temp_quad;
	temp_quad.quad_id = quad_id;
	temp_quad.nd1 = nd1;
	temp_quad.nd2 = nd2;
	temp_quad.nd3 = nd3;
	temp_quad.nd4 = nd4;

	// Add the point coordinate
	temp_quad.nd1pt = (*nd1).node_pt;
	temp_quad.nd2pt = (*nd2).node_pt;
	temp_quad.nd3pt = (*nd3).node_pt;
	temp_quad.nd4pt = (*nd4).node_pt;
	temp_quad.midpt = midpt;

	// Add the modal displacement
	temp_quad.nd1_displ = (*nd1).node_pulse_result.node_displ;
	temp_quad.nd2_displ = (*nd2).node_pulse_result.node_displ;
	temp_quad.nd3_displ = (*nd3).node_pulse_result.node_displ;
	temp_quad.nd4_displ = (*nd4).node_pulse_result.node_displ;
	temp_quad.midpt_displ = midpt_displ;

	// Add the modal displacement magnitude
	temp_quad.nd1_displ_mag = (*nd1).node_pulse_result.node_displ_magnitude;
	temp_quad.nd2_displ_mag = (*nd2).node_pulse_result.node_displ_magnitude;
	temp_quad.nd3_displ_mag = (*nd3).node_pulse_result.node_displ_magnitude;
	temp_quad.nd4_displ_mag = (*nd4).node_pulse_result.node_displ_magnitude;
	temp_quad.midpt_displ_mag = midpt_displ_mag;


	// Check whether the quad_id is already there
	if (pulse_elementquadMap.find(quad_id) != pulse_elementquadMap.end())
	{
		// Element ID already exist (do not add)
		return;
	}

	// Insert to the Quads
	pulse_elementquadMap.insert({ quad_id, temp_quad });
	pulse_elementquad_count++;

}

void pulse_elementquad_list_store::set_buffer()
{
	// Clear existing modal quads (8 triangles)
	pulse_element_tris1m2.clear_triangles(); // Tri 1m2
	pulse_element_tris2m3.clear_triangles(); // Tri 2m3
	pulse_element_tris3m4.clear_triangles(); // Tri 3m4
	pulse_element_tris4m1.clear_triangles(); // Tri 4m1

	// Add the lines
	// Loop throug every line element
	int i = 0;
	for (auto& quad_m : pulse_elementquadMap)
	{
		pulse_elementquad_store quad = quad_m.second;

		// Scale the displacement with the max pulse displacement
		std::vector<glm::vec3> node1_displ;
		std::vector<double> node1_displ_magnitude;
		std::vector<glm::vec3> node2_displ;
		std::vector<double> node2_displ_magnitude;
		std::vector<glm::vec3> node3_displ;
		std::vector<double> node3_displ_magnitude;
		std::vector<glm::vec3> node4_displ;
		std::vector<double> node4_displ_magnitude;
		std::vector<glm::vec3> mid_displ;
		std::vector<double> mid_displ_magnitude;


		for (int i = 0; i < static_cast<int>(quad.nd1_displ.size()); i++)
		{
			// Node 1
			node1_displ.push_back(quad.nd1_displ[i] / static_cast<float> (max_quad_displ));
			node1_displ_magnitude.push_back(quad.nd1_displ_mag[i]/ static_cast<float> (max_quad_displ ));

			// Node 2
			node2_displ.push_back(quad.nd2_displ[i]/ static_cast<float> (max_quad_displ));
			node2_displ_magnitude.push_back(quad.nd2_displ_mag[i] / static_cast<float> (max_quad_displ));

			// Node 3
			node3_displ.push_back(quad.nd3_displ[i] / static_cast<float> (max_quad_displ));
			node3_displ_magnitude.push_back(quad.nd3_displ_mag[i]/ static_cast<float> (max_quad_displ));

			// Node 4
			node4_displ.push_back(quad.nd4_displ[i] / static_cast<float> (max_quad_displ));
			node4_displ_magnitude.push_back(quad.nd4_displ_mag[i] / static_cast<float> (max_quad_displ));

			// Mid pt
			mid_displ.push_back(quad.midpt_displ[i] / static_cast<float> (max_quad_displ));
			mid_displ_magnitude.push_back(quad.midpt_displ_mag[i]/ static_cast<float> (max_quad_displ));
		}


		// Add to the 8 triangle list
		pulse_element_tris1m2.add_tri(i, quad.nd1pt, quad.midpt, quad.nd2pt,
			node1_displ, mid_displ, node2_displ,
			node1_displ_magnitude, mid_displ_magnitude, node2_displ_magnitude); // Tri 1m2
		pulse_element_tris2m3.add_tri(i, quad.nd2pt, quad.midpt, quad.nd3pt,
			node2_displ, mid_displ, node3_displ,
			node2_displ_magnitude, mid_displ_magnitude, node3_displ_magnitude); // Tri 2m3
		pulse_element_tris3m4.add_tri(i, quad.nd3pt, quad.midpt, quad.nd4pt,
			node3_displ, mid_displ, node4_displ,
			node3_displ_magnitude, mid_displ_magnitude, node4_displ_magnitude); // Tri 3m4
		pulse_element_tris4m1.add_tri(i, quad.nd4pt, quad.midpt, quad.nd1pt,
			node4_displ, mid_displ, node1_displ,
			node4_displ_magnitude, mid_displ_magnitude, node1_displ_magnitude); // Tri 4m1


		i++;
	}

	// Set the buffer
	pulse_element_tris1m2.set_buffer(); // Tri 1m2
	pulse_element_tris2m3.set_buffer(); // Tri 2m3
	pulse_element_tris3m4.set_buffer(); // Tri 3m4
	pulse_element_tris4m1.set_buffer(); // Tri 4m1

}

void pulse_elementquad_list_store::paint_pulse_elementquads(const int& dyn_index)
{
	// Paint the triangles
	pulse_element_tris1m2.paint_triangles(dyn_index); // Tri 1m2
	pulse_element_tris2m3.paint_triangles(dyn_index); // Tri 2m3
	pulse_element_tris3m4.paint_triangles(dyn_index); // Tri 3m4
	pulse_element_tris4m1.paint_triangles(dyn_index); // Tri 4m1
}

void pulse_elementquad_list_store::update_geometry_matrices(bool set_modelmatrix, bool set_pantranslation, bool set_rotatetranslation, bool set_zoomtranslation, bool set_transparency, bool set_deflscale)
{
	// Result pulse quad update geometry 
	pulse_element_tris1m2.update_opengl_uniforms(set_modelmatrix, set_pantranslation, set_rotatetranslation,
		set_zoomtranslation, set_transparency, set_deflscale);
	pulse_element_tris2m3.update_opengl_uniforms(set_modelmatrix, set_pantranslation, set_rotatetranslation,
		set_zoomtranslation, set_transparency, set_deflscale);
	pulse_element_tris3m4.update_opengl_uniforms(set_modelmatrix, set_pantranslation, set_rotatetranslation,
		set_zoomtranslation, set_transparency, set_deflscale);
	pulse_element_tris4m1.update_opengl_uniforms(set_modelmatrix, set_pantranslation, set_rotatetranslation,
		set_zoomtranslation, set_transparency, set_deflscale);

}
