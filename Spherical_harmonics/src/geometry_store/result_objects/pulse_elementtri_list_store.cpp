#include "pulse_elementtri_list_store.h"

pulse_elementtri_list_store::pulse_elementtri_list_store()
{
	// Empty constructor
}

pulse_elementtri_list_store::~pulse_elementtri_list_store()
{
	// Empty destructor
}

void pulse_elementtri_list_store::init(geom_parameters* geom_param_ptr)
{
	// Set the geometry parameters
	this->geom_param_ptr = geom_param_ptr;

	// Set the geometry parameters for the tris (and clear the tris)
	pulse_element_tris.init(geom_param_ptr); // Tri 

	// Clear the element tris
	pulse_elementtri_count = 0;
	pulse_elementtriMap.clear();

}

void pulse_elementtri_list_store::clear_data()
{
	// Clear the element tri
	pulse_element_tris.clear_triangles(); // Tri

	// Clear the element tris
	pulse_elementtri_count = 0;
	pulse_elementtriMap.clear();

}

void pulse_elementtri_list_store::add_pulse_elementtriangle(int& tri_id, pulse_node_store* nd1, pulse_node_store* nd2, pulse_node_store* nd3)
{
	// Add result tri element
	pulse_elementtri_store temp_tri;
	temp_tri.tri_id = tri_id;
	temp_tri.nd1 = nd1;
	temp_tri.nd2 = nd2;
	temp_tri.nd3 = nd3;

	// Add the point coordinate
	temp_tri.nd1pt = (*nd1).node_pt;
	temp_tri.nd2pt = (*nd2).node_pt;
	temp_tri.nd3pt = (*nd3).node_pt;

	// Add the modal displacement
	temp_tri.nd1_displ = (*nd1).node_pulse_result.node_displ;
	temp_tri.nd2_displ = (*nd2).node_pulse_result.node_displ;
	temp_tri.nd3_displ = (*nd3).node_pulse_result.node_displ;

	// Add the modal displacement magnitude
	temp_tri.nd1_displ_mag = (*nd1).node_pulse_result.node_displ_magnitude;
	temp_tri.nd2_displ_mag = (*nd2).node_pulse_result.node_displ_magnitude;
	temp_tri.nd3_displ_mag = (*nd3).node_pulse_result.node_displ_magnitude;

	// Check whether the tri_id is already there
	if (pulse_elementtriMap.find(tri_id) != pulse_elementtriMap.end())
	{
		// Element ID already exist (do not add)
		return;
	}

	// Insert to the tris
	pulse_elementtriMap.insert({ tri_id, temp_tri });
	pulse_elementtri_count++;

}

void pulse_elementtri_list_store::set_buffer()
{
	// Clear existing modal tris (1 triangle)
	pulse_element_tris.clear_triangles(); // Tri
	
	// Add the tris
	// Loop throug every tri element
	int i = 0;
	for (auto& tri_m : pulse_elementtriMap)
	{
		pulse_elementtri_store tri = tri_m.second;

		// Scale the displacement with the max pulse displacement
		std::vector<glm::vec3> node1_displ;
		std::vector<double> node1_displ_magnitude;
		std::vector<glm::vec3> node2_displ;
		std::vector<double> node2_displ_magnitude;
		std::vector<glm::vec3> node3_displ;
		std::vector<double> node3_displ_magnitude;

		for (int i = 0; i < static_cast<int>(tri.nd1_displ.size()); i++)
		{
			// Node 1
			node1_displ.push_back(tri.nd1_displ[i] / static_cast<float> (max_tri_displ));
			node1_displ_magnitude.push_back(tri.nd1_displ_mag[i] / static_cast<float> (max_tri_displ));

			// Node 2
			node2_displ.push_back(tri.nd2_displ[i] / static_cast<float> (max_tri_displ));
			node2_displ_magnitude.push_back(tri.nd2_displ_mag[i] / static_cast<float> (max_tri_displ));

			// Node 3
			node3_displ.push_back(tri.nd3_displ[i] / static_cast<float> (max_tri_displ));
			node3_displ_magnitude.push_back(tri.nd3_displ_mag[i] / static_cast<float> (max_tri_displ));

		}


		// Add to the 1 triangle list
		pulse_element_tris.add_tri(i, tri.nd1pt, tri.nd2pt, tri.nd3pt,
			node1_displ, node2_displ, node3_displ,
			node1_displ_magnitude, node2_displ_magnitude, node3_displ_magnitude); // Tri

		i++;
	}

	// Set the buffer
	pulse_element_tris.set_buffer(); // Tri

}

void pulse_elementtri_list_store::paint_pulse_elementtriangles(const int& dyn_index)
{
	// Paint the triangle
	pulse_element_tris.paint_triangles(dyn_index); // Tri

}

void pulse_elementtri_list_store::update_geometry_matrices(bool set_modelmatrix, bool set_pantranslation, bool set_rotatetranslation, bool set_zoomtranslation, bool set_transparency, bool set_deflscale)
{
	// Result pulse quad update geometry 
	pulse_element_tris.update_opengl_uniforms(set_modelmatrix, set_pantranslation, set_rotatetranslation,
		set_zoomtranslation, set_transparency, set_deflscale);
}
