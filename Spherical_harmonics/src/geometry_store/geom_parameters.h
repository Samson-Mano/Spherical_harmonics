#pragma once
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/string_cast.hpp>
#include <unordered_set>
#include <sstream>
#include <vector>
#include <set>
#include "geometry_buffers/font_atlas.h"

// Stopwatch
#include "../events_handler/Stopwatch_events.h"

struct geom_color_theme
{
	glm::vec3 background_color = glm::vec3(0);
	glm::vec3 point_color = glm::vec3(0);
	glm::vec3 edge_color = glm::vec3(0);
	glm::vec3 triangle_color = glm::vec3(0);

	glm::vec3 selection_color = glm::vec3(0);
	glm::vec3 load_color = glm::vec3(0);
	glm::vec3 constraint_color = glm::vec3(0);
	glm::vec3 ptmass_color = glm::vec3(0);
	glm::vec3 inlcond_displ_color = glm::vec3(0);
	glm::vec3 inlcond_velo_color = glm::vec3(0);
};

struct chart_setting_data
{
	// X Range
	float chart_x_max = 0.0f;
	float chart_x_min = 0.0f;

	// Y Range
	float chart_y_max = 0.0f;
	float chart_y_min = 0.0f;

	// Number of data points
	int data_pt_count = 0;
};

struct frequency_reponse_data
{
	int node_id;
	std::vector<double> frequency_values;
	// X response
	std::vector<double> displ_x;
	std::vector<double> phase_x;

	// Y response
	std::vector<double> displ_y;
	std::vector<double> phase_y;

	// Magnitude response
	std::vector<double> displ_magnitude;
	std::vector<double> phase_magnitude;

};


struct material_data
{
	unsigned int material_id = 0;
	std::string material_name = "";
	double line_length = 0.0;
	double line_tension = 0.0;
	double material_density = 0.0;
	int model_type = -1;
};


class geom_parameters
{
public:
	// Standard sizes
	const float font_size = static_cast<float>(12.0f * std::pow(10, -5));
	const float node_circle_radii = 0.005f;

	// Geometry size
	const float point_size = 2.4f;
	const float selected_point_size = 4.4f;
	const float line_width = 1.1f;
	const float selected_line_width = 2.1f;
	
	// Precision for various values
	const int length_precision = 2;
	const int coord_precision = 2;
	const int constraint_precision = 2;
	const int load_precision = 2;
	const int inlcond_precision = 3;
	const int defl_precision = 6;

	// Triangle mesh shrunk factor
	const double traingle_shrunk_factor = 0.8;

	// File path
	std::filesystem::path resourcePath = "";

	// Window size
	int window_width = 0;
	int window_height = 0;

	glm::vec3 min_b = glm::vec3(0); // (min_x, min_y)
	glm::vec3 max_b = glm::vec3(0); // (max_x, max_y)
	glm::vec3 geom_bound = glm::vec3(0); // Bound magnitude
	glm::vec3 center = glm::vec3(0); // center of the geometry
	glm::mat4 modelMatrix = glm::mat4(0); // Geometry model matrix
	glm::quat default_transl = glm::quat(0.4402697668541200f, 0.8215545196058330f, 0.2968766167094340f, -0.2075451231915790f); // Default translation

	double geom_scale = 0.0; // Scale of the geometry
	double geom_transparency = 1.0; // Value to control the geometry transparency
	double normalized_defl_scale = 0.0f; // Value of deflection scale
	double defl_scale = 0.0f; // Value of deflection scale


	// Screen transformations
	glm::mat4 viewMatrix = glm::mat4(1.0f); // View matrix
	glm::mat4 projectionMatrix = glm::mat4(1.0f); // Projection matrix
	glm::mat4 panTranslation = glm::mat4(1.0f); // Pan translataion
	glm::mat4 rotateTranslation = glm::mat4(1.0f); // Rotate translation
	double zoom_scale = 0.0; // Zoom scale

	// Standard colors
	geom_color_theme geom_colors;

	font_atlas main_font;

	geom_parameters();
	~geom_parameters();
	void init();

	static bool isPointInsideRectangle(const glm::vec2& rect_cpt1, const glm::vec2& rect_cpt2, const glm::vec2& pt);

	static void copyNodenumberlistToCharArray(const std::vector<int>& vec, char* charArray, size_t bufferSize);

	static glm::vec3 get_standard_color(int color_index);

	static glm::vec2 linear_interpolation(const glm::vec2& pt1, const glm::vec2& pt2, const double& param_t);

	static glm::vec3 linear_interpolation3d(const glm::vec3& pt1, const glm::vec3& pt2, const double& param_t);

	static	glm::vec3 findGeometricCenter(const std::vector<glm::vec3>& all_pts);

	static	glm::vec3 findGeometricCenter(const glm::vec3& pt1, const glm::vec3& pt2, const glm::vec3& pt3);

	static	glm::vec3 findGeometricCenter(const glm::vec3& pt1, const glm::vec3& pt2, const glm::vec3& pt3, const glm::vec3& pt4);

	static std::pair<glm::vec3, glm::vec3> findMinMaxXY(const std::vector<glm::vec3>& all_pts);

	static glm::vec3 getHeatMapColor(float value);

	static glm::vec3 getContourColor_d(float value);

	static double get_triangle_area(const glm::vec2& pt1, const glm::vec2& pt2, const glm::vec2& pt3);

	static double get_line_length(const glm::vec2& pt1, const glm::vec2& pt2);

	static glm::vec2 calculateCatmullRomPoint(const std::vector<glm::vec2>& controlPoints, float t);

	static double get_lerp(const double& max_value, const double& min_value, const double& param_t);

	static double get_invlerp(const double& max_value, const double& min_value, const double& value);

	static double get_remap(const double& max_value, const double& min_value, const double& limit_max, 
		const double& limit_min,		const double& value);

	static double roundToSixDigits(const double& number);

	static glm::vec3 get_face_normal(const glm::vec3& pt1, const glm::vec3& pt2, const glm::vec3& pt3);

	static glm::vec3 get_face_normal(const glm::vec3& pt1, const glm::vec3& pt2, const glm::vec3& pt3, const glm::vec3& pt4);

private:
	static double HueToRGB(double v1, double v2, double vH);

};



