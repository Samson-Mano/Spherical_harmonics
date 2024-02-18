#include "elementquad_list_store.h"

elementquad_list_store::elementquad_list_store()
{
	// Empty Constructor
}

elementquad_list_store::~elementquad_list_store()
{
	// Empty Destructor
}

void elementquad_list_store::init(geom_parameters* geom_param_ptr, dcel_mesh_data* mesh_data)
{
	// Set the geometry parameters
	this->geom_param_ptr = geom_param_ptr;
	this->mesh_data = mesh_data;

	//// Set the geometry parameters for the labels (and clear the labels)
	//element_tris123.init(geom_param_ptr); // Tri 123
	//element_tris134.init(geom_param_ptr); // Tri 134
	//element_tris123_shrunk.init(geom_param_ptr); // Tri shrunk 123
	//element_tris134_shrunk.init(geom_param_ptr); // Tri shrunk 134
	//element_materialid.init(geom_param_ptr);

	// Clear the triangles
	elementquad_count = 0;
	elementquadMap.clear();
}

void elementquad_list_store::add_elementquadrilateral(int& quad_id, node_store* nd1, node_store* nd2, node_store* nd3, node_store* nd4)
{
	// Add the quadrilateral to the list
	elementquad_store temp_quad;
	temp_quad.quad_id = quad_id; // Quadrilateral ID
	temp_quad.nd1 = nd1;
	temp_quad.nd2 = nd2;
	temp_quad.nd3 = nd3;
	temp_quad.nd4 = nd4;

	//// Check whether the node_id is already there
	//if (elementquadMap.find(quad_id) != elementquadMap.end())
	//{
	//	// Element ID already exist (do not add)
	//	return;
	//}

	// Insert to the quadrilaterals
	elementquadMap.insert({ quad_id, temp_quad });
	elementquad_count++;

	//__________________________ Add the Quadrilateral
	mesh_data->add_mesh_quadrilateral(quad_id, nd1->node_id, nd2->node_id, nd3->node_id, nd4->node_id);



	////__________________________ Add the Triangle
	//int tri_id123 = element_tris123.tri_count;
	//int tri_id134 = element_tris134.tri_count;
	//glm::vec3 node_pt1 = (*nd1).node_pt; // Node pt 1
	//glm::vec3 node_pt2 = (*nd2).node_pt; // Node pt 2
	//glm::vec3 node_pt3 = (*nd3).node_pt; // Node pt 3
	//glm::vec3 node_pt4 = (*nd4).node_pt; // Node pt 4

	//glm::vec3 temp_tri_color = geom_param_ptr->geom_colors.triangle_color;
	//// Main triangle 123
	//element_tris123.add_tri(tri_id123, node_pt1, node_pt2, node_pt3,
	//									temp_tri_color, temp_tri_color, temp_tri_color);

	//// Main triangle 134
	//element_tris134.add_tri(tri_id134, node_pt1, node_pt3, node_pt4,
	//									temp_tri_color, temp_tri_color, temp_tri_color);

	//// Find the shrunk vertices
	//glm::vec3 midpt = glm::vec3((node_pt1.x + node_pt2.x + node_pt3.x + node_pt4.x) / 4.0f,
	//	(node_pt1.y + node_pt2.y + node_pt3.y + node_pt4.y) / 4.0f,
	//	(node_pt1.z + node_pt2.z + node_pt3.z + node_pt4.z) / 4.0f);
	//double shrink_factor = geom_param_ptr->traingle_shrunk_factor;

	//glm::vec3 shrunk_node_pt1 = geom_parameters::linear_interpolation3d(midpt, node_pt1, shrink_factor);
	//glm::vec3 shrunk_node_pt2 = geom_parameters::linear_interpolation3d(midpt, node_pt2, shrink_factor);
	//glm::vec3 shrunk_node_pt3 = geom_parameters::linear_interpolation3d(midpt, node_pt3, shrink_factor);
	//glm::vec3 shrunk_node_pt4 = geom_parameters::linear_interpolation3d(midpt, node_pt4, shrink_factor);

	//// Main triangle123 as shrunk
	//element_tris123_shrunk.add_tri(tri_id123, shrunk_node_pt1, shrunk_node_pt2, shrunk_node_pt3,
	//											temp_tri_color, temp_tri_color, temp_tri_color);

	//// Main triangle134 as shrunk
	//element_tris134_shrunk.add_tri(tri_id134, shrunk_node_pt1, shrunk_node_pt3, shrunk_node_pt4,
	//											temp_tri_color, temp_tri_color, temp_tri_color);


}


//void elementquad_list_store::set_buffer()
//{
//	// Set the buffers for the Model
//	// Quad
//	element_tris123.set_buffer();
//	element_tris134.set_buffer();
//	// shrunk quad
//	element_tris123_shrunk.set_buffer();
//	element_tris134_shrunk.set_buffer();
//}
//
//void elementquad_list_store::paint_elementquadrilaterals()
//{
//	// Paint the Quadrilaterals
//	element_tris123.paint_triangles();
//	element_tris134.paint_triangles();
//}
//
//
//void elementquad_list_store::paint_elementquadrilaterals_shrunk()
//{
//	// Paint the quadrilaterals shrunk
//	element_tris123_shrunk.paint_triangles();
//	element_tris134_shrunk.paint_triangles();
//}
//
//void elementquad_list_store::update_geometry_matrices(bool set_modelmatrix, bool set_pantranslation, bool set_rotatetranslation,
//	bool set_zoomtranslation, bool set_transparency, bool set_deflscale)
//{
//	// Update model openGL uniforms
//	element_tris123.update_opengl_uniforms(set_modelmatrix, set_pantranslation, set_rotatetranslation, set_zoomtranslation, set_transparency, set_deflscale);
//	element_tris134.update_opengl_uniforms(set_modelmatrix, set_pantranslation, set_rotatetranslation, set_zoomtranslation, set_transparency, set_deflscale);
//	element_tris123_shrunk.update_opengl_uniforms(set_modelmatrix, set_pantranslation, set_rotatetranslation, set_zoomtranslation, set_transparency, set_deflscale);
//	element_tris134_shrunk.update_opengl_uniforms(set_modelmatrix, set_pantranslation, set_rotatetranslation, set_zoomtranslation, set_transparency, set_deflscale);
//	element_materialid.update_opengl_uniforms(set_modelmatrix, set_pantranslation, set_rotatetranslation, set_zoomtranslation, set_transparency, set_deflscale);
//	
//}
