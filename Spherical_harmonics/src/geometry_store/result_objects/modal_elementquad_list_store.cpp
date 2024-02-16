#include "modal_elementquad_list_store.h"

modal_elementquad_list_store::modal_elementquad_list_store()
{
	// Empty Constructor
}

modal_elementquad_list_store::~modal_elementquad_list_store()
{
	// Empty Destructor
}

void modal_elementquad_list_store::init(geom_parameters* geom_param_ptr)
{
	// Set the geometry parameters
	this->geom_param_ptr = geom_param_ptr;

	// Set the geometry parameters for the quads (and clear the quads)
	modal_element_tris1m2.init(geom_param_ptr); // Tri 1m2
	modal_element_tris2m3.init(geom_param_ptr); // Tri 2m3
	modal_element_tris3m4.init(geom_param_ptr); // Tri 3m4
	modal_element_tris4m1.init(geom_param_ptr); // Tri 4m1

	// Clear the element quads
	modal_elementquad_count = 0;
	modal_elementquadMap.clear();
}

void modal_elementquad_list_store::clear_data()
{
	// Clear the element quads
	modal_element_tris1m2.clear_triangles(); // Tri 1m2
	modal_element_tris2m3.clear_triangles(); // Tri 2m3
	modal_element_tris3m4.clear_triangles(); // Tri 3m4
	modal_element_tris4m1.clear_triangles(); // Tri 4m1

	// Clear the element quads
	modal_elementquad_count = 0;
	modal_elementquadMap.clear();

}

void modal_elementquad_list_store::add_modal_elementquadrilateral(int& quad_id, modal_node_store* nd1,
	modal_node_store* nd2, modal_node_store* nd3, modal_node_store* nd4, 
	glm::vec3& midpt, std::vector<glm::vec3>& midpt_modal_displ, std::vector<double>& midpt_modal_displ_mag)
{
	// Add result quad element
	modal_elementquad_store temp_quad;
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
	temp_quad.nd1_modal_displ = (*nd1).node_modal_displ;
	temp_quad.nd2_modal_displ = (*nd2).node_modal_displ;
	temp_quad.nd3_modal_displ = (*nd3).node_modal_displ;
	temp_quad.nd4_modal_displ = (*nd4).node_modal_displ;
	temp_quad.midpt_modal_displ = midpt_modal_displ;

	// Add the modal displacement magnitude
	temp_quad.nd1_modal_displ_mag = (*nd1).node_modal_displ_magnitude;
	temp_quad.nd2_modal_displ_mag = (*nd2).node_modal_displ_magnitude;
	temp_quad.nd3_modal_displ_mag = (*nd3).node_modal_displ_magnitude;
	temp_quad.nd4_modal_displ_mag = (*nd4).node_modal_displ_magnitude;
	temp_quad.midpt_modal_displ_mag = midpt_modal_displ_mag;


	// Check whether the Quad_id is already there
	if (modal_elementquadMap.find(quad_id) != modal_elementquadMap.end())
	{
		// Element ID already exist (do not add)
		return;
	}

	// Insert to the Quads
	modal_elementquadMap.insert({ quad_id, temp_quad });
	modal_elementquad_count++;

}

void modal_elementquad_list_store::set_buffer()
{
	// Clear existing modal quads (8 triangles)
	modal_element_tris1m2.clear_triangles(); // Tri 1m2
	modal_element_tris2m3.clear_triangles(); // Tri 2m3
	modal_element_tris3m4.clear_triangles(); // Tri 3m4
	modal_element_tris4m1.clear_triangles(); // Tri 4m1

	// Add the lines
	// Loop throug every line element
	int i = 0;
	for (auto& quad_m : modal_elementquadMap)
	{
		modal_elementquad_store quad = quad_m.second;


		// Add to the 8 triangle list
		modal_element_tris1m2.add_tri(i, quad.nd1pt, quad.midpt, quad.nd2pt,
			quad.nd1_modal_displ, quad.midpt_modal_displ, quad.nd2_modal_displ,
			quad.nd1_modal_displ_mag, quad.midpt_modal_displ_mag, quad.nd2_modal_displ_mag); // Tri 1m2
		modal_element_tris2m3.add_tri(i, quad.nd2pt, quad.midpt, quad.nd3pt,
			quad.nd2_modal_displ, quad.midpt_modal_displ, quad.nd3_modal_displ,
			quad.nd2_modal_displ_mag, quad.midpt_modal_displ_mag, quad.nd3_modal_displ_mag); // Tri 2m3
		modal_element_tris3m4.add_tri(i, quad.nd3pt, quad.midpt, quad.nd4pt,
			quad.nd3_modal_displ, quad.midpt_modal_displ, quad.nd4_modal_displ,
			quad.nd3_modal_displ_mag, quad.midpt_modal_displ_mag, quad.nd4_modal_displ_mag); // Tri 3m4
		modal_element_tris4m1.add_tri(i, quad.nd4pt, quad.midpt, quad.nd1pt,
			quad.nd4_modal_displ, quad.midpt_modal_displ, quad.nd1_modal_displ,
			quad.nd4_modal_displ_mag, quad.midpt_modal_displ_mag, quad.nd1_modal_displ_mag); // Tri 4m1


		i++;
	}

	// Set the buffer
	modal_element_tris1m2.set_buffer(); // Tri 1m2
	modal_element_tris2m3.set_buffer(); // Tri 2m3
	modal_element_tris3m4.set_buffer(); // Tri 3m4
	modal_element_tris4m1.set_buffer(); // Tri 4m1

}


void modal_elementquad_list_store::update_buffer(int selected_mode)
{
	// Update the buffer based on the selected mode
	modal_element_tris1m2.update_buffer(selected_mode); // Tri 1m2
	modal_element_tris2m3.update_buffer(selected_mode); // Tri 2m3
	modal_element_tris3m4.update_buffer(selected_mode); // Tri 3m4
	modal_element_tris4m1.update_buffer(selected_mode); // Tri 4m1

}


void modal_elementquad_list_store::paint_modal_elementquadrilaterals()
{
	// Paint the triangles
	modal_element_tris1m2.paint_triangles(); // Tri 1m2
	modal_element_tris2m3.paint_triangles(); // Tri 2m3
	modal_element_tris3m4.paint_triangles(); // Tri 3m4
	modal_element_tris4m1.paint_triangles(); // Tri 4m1

}

void modal_elementquad_list_store::update_geometry_matrices(bool set_modelmatrix, bool set_pantranslation,
	bool set_rotatetranslation, bool set_zoomtranslation, bool set_transparency, bool set_deflscale)
{
	// Result quad update geometry 
	modal_element_tris1m2.update_opengl_uniforms(set_modelmatrix, set_pantranslation, set_rotatetranslation,
		set_zoomtranslation, set_transparency, set_deflscale);
	modal_element_tris2m3.update_opengl_uniforms(set_modelmatrix, set_pantranslation, set_rotatetranslation,
		set_zoomtranslation, set_transparency, set_deflscale);
	modal_element_tris3m4.update_opengl_uniforms(set_modelmatrix, set_pantranslation, set_rotatetranslation,
		set_zoomtranslation, set_transparency, set_deflscale);
	modal_element_tris4m1.update_opengl_uniforms(set_modelmatrix, set_pantranslation, set_rotatetranslation,
		set_zoomtranslation, set_transparency, set_deflscale);
}
