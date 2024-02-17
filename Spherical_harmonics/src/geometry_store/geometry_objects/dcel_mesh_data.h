#pragma once
#include "point_list_store.h"

struct vertex_store
{
	int vertex_id = -1; // vertex id
	double x = 0.0; // x coordinate
	double y = 0.0; // y coordinate
	double z = 0.0; // z coordinate
};

struct triangle_store; // Forward declaration

struct edge_store
{
	int edge_id = -1;
	vertex_store* origin = nullptr;
	vertex_store* destination = nullptr;
	edge_store* next = nullptr;        // Next half-edge in the same face
	edge_store* twin = nullptr;        // Opposite half-edge
	triangle_store* face = nullptr;    // Face to the left of this half-edge
};

struct triangle_store 
{
	int tri_id = -1;
	edge_store* edge1 = nullptr; // Triangle edge 1
	edge_store* edge2 = nullptr; // Triangle edge 2
	edge_store* edge3 = nullptr; // Triangle edge 3
};




class dcel_mesh_data
{
public:
	geom_parameters* geom_param_ptr = nullptr;
	int point_count = 0;
	int half_edge_count = 0;
	int edge_count = 0;
	int tria_count = 0;
	std::unordered_map<int, int> pointId_Map; // store the point Id and vector index
	std::vector<vertex_store> pointMap; // All the points
	std::vector<triangle_store> triMap; // All the triangles
	std::vector<edge_store*> HalfedgeMap; // All the Half edge data
	std::vector<edge_store*> edgeMap; // All the unique edge data

	dcel_mesh_data();
	~dcel_mesh_data();
	void init(geom_parameters* geom_param_ptr);

	void add_points(const int& point_id,
		const double& x_coord,
		const double& y_coord,
		const double& z_coord);

	void add_triangles(const int& tri_id,
		const int& point_id1,
		const int& point_id2,
		const int& point_id3);

	void clear_mesh();

	void set_buffer();

	void paint_points();
	void paint_selected_points();
	void paint_edges();
	void paint_triangles();

	void update_opengl_uniforms(bool set_modelmatrix, bool set_pantranslation, bool set_rotatetranslation,
		bool set_zoomtranslation, bool set_transparency);
private:
	Shader point_shader;
	Shader edge_shader;
	Shader tria_shader;
	gBuffers point_buffer;
	gBuffers edge_buffer;
	gBuffers tria_buffer;


	int add_edges(const int& startPt_id,
		const int& endPt_id);

	void set_point_buffer();
	void set_edge_buffer();
	void set_triangle_buffer();

	void get_point_buffer(const vertex_store& pt,float* point_vertices, unsigned int& point_v_index,
		unsigned int* point_vertex_indices, unsigned int& point_i_index);

	void get_edge_buffer(const edge_store* ln, float* edge_vertices, unsigned int& edge_v_index,
		unsigned int* edge_vertex_indices, unsigned int& edge_i_index);

	void get_tria_buffer(const triangle_store& tri ,float* tri_vertices, unsigned int& tri_v_index,
		unsigned int* tri_vertex_indices, unsigned int& tri_i_index);

	std::vector<edge_store*> getUniqueEdges(const std::vector<triangle_store>& triangles);

	void addUniqueEdgeToSet(std::vector<edge_store*>* uniqueEdges, edge_store* edge);

	void updateTwinEdges(std::vector<triangle_store>& triangles);

	void checkAndUpdateTwin(std::unordered_map<std::pair<vertex_store*, vertex_store*>, edge_store*>* edgeMap, edge_store* edge);
};

/*


// Function to get unique edges from a vector of triangles
std::vector<edge_store*> getUniqueEdges(const std::vector<triangle_store>& triangles) {
	std::unordered_set<std::pair<vertex_store*, vertex_store*>> edgeSet;
	std::vector<edge_store*> uniqueEdges;

	// Iterate over all triangles
	for (const auto& triangle : triangles) {
		// Add each edge of the triangle to the set of unique edges
		addUniqueEdgeToSet(&edgeSet, &uniqueEdges, triangle.edge1);
		addUniqueEdgeToSet(&edgeSet, &uniqueEdges, triangle.edge2);
		addUniqueEdgeToSet(&edgeSet, &uniqueEdges, triangle.edge3);
	}

	return uniqueEdges;
}

// Function to add an edge to the set of unique edges
void addUniqueEdgeToSet(std::unordered_set<std::pair<vertex_store*, vertex_store*>>* edgeSet, std::vector<edge_store*>* uniqueEdges, edge_store* edge) {
	if (edge == nullptr)
		return;

	// Check if the edge already exists in the set
	auto result = edgeSet->insert({edge->origin, edge->destination});

	// If the edge is unique, add it to the vector of unique edges
	if (result.second) {
		uniqueEdges->push_back(edge);
	}
}



void updateTwinEdges(std::vector<triangle_store>& triangles) {
	// Create a map to store edges based on their origin and destination vertices
	std::unordered_map<std::pair<vertex_store*, vertex_store*>, edge_store*> edgeMap;

	// Iterate over all triangles
	for (auto& triangle : triangles) {
		// Check each edge of the triangle
		checkAndUpdateTwin(&edgeMap, triangle.edge1);
		checkAndUpdateTwin(&edgeMap, triangle.edge2);
		checkAndUpdateTwin(&edgeMap, triangle.edge3);
	}
}

void checkAndUpdateTwin(std::unordered_map<std::pair<vertex_store*, vertex_store*>, edge_store*>* edgeMap, edge_store* edge) {
	if (edge == nullptr || edge->twin != nullptr) // Skip if edge is null or twin is already set
		return;

	// Check if the twin of the edge exists in the map
	auto twinIt = edgeMap->find({ edge->destination, edge->origin });

	if (twinIt != edgeMap->end()) {
		// Found twin in the map, update twin pointers for both edges
		edge->twin = twinIt->second;
		twinIt->second->twin = edge;
	}
	else {
		// Twin not found, add the edge to the map
		(*edgeMap)[{edge->origin, edge->destination}] = edge;
	}
}

*/