#include "geom_parameters.h"

geom_parameters::geom_parameters()
{
	// Empty Constructor
}

geom_parameters::~geom_parameters()
{
	// Empty Destructor
}

void geom_parameters::init()
{
	// Initialize the paramters
	resourcePath = std::filesystem::current_path();
	std::cout << "Current path of application is " << resourcePath << std::endl;

	// Create the font atlas
	main_font.create_atlas(resourcePath);

	// Initialize the color theme
	//geom_colors.background_color = glm::vec3(0.62f, 0.62f, 0.62f); // White
	//geom_colors.selection_color = glm::vec3(0.862745f, 0.078431f, 0.23529f); // Crimson
	//geom_colors.constraint_color = glm::vec3(0.0f, 0.50196f, 0.0f); // Green
	//geom_colors.load_color = glm::vec3(0.6f, 0.0f, 0.6f);
	//geom_colors.ptmass_color = glm::vec3(0.82f, 0.77f, 0.92f);
	//geom_colors.inlcond_displ_color = glm::vec3(0.96f, 0.5f, 0.1f);
	//geom_colors.inlcond_velo_color = glm::vec3(0.54f, 0.06f, 0.31f);
	//geom_colors.node_color = glm::vec3(0.54509f, 0.0f, 0.4f); // Dark Red
	//geom_colors.line_color = glm::vec3(1.0f, 0.54901f, 0.6f); // Dark Orange

	geom_colors.point_color = glm::vec3(0.8f, 0.2f, 0.2f); // Light Red
	geom_colors.edge_color = glm::vec3(1.0f, 0.54901f, 0.6f); // Dark Orange
	geom_colors.triangle_color = glm::vec3(0.90196f, 0.90196f, 0.98039f); // Lavender

	geom_colors.ptmass_color = glm::vec3(0.2f, 0.7f, 0.2f); // Green
	geom_colors.constraint_color = glm::vec3(0.5f, 0.5f, 0.5f); // Gray
	geom_colors.load_color = glm::vec3(0.6f, 0.0f, 0.6f);


	geom_colors.background_color = glm::vec3(0.95f, 0.95f, 0.95f); // Light gray
	geom_colors.selection_color = glm::vec3(0.862745f, 0.078431f, 0.23529f); // Crimson

	geom_colors.inlcond_displ_color = glm::vec3(0.54f, 0.06f, 0.31f); 
	geom_colors.inlcond_velo_color = glm::vec3(0.2, 0.7, 0.2); // Green



	// Assuming viewer position is (0, 0, 0)
	// Camera position is (-1, 0, 0) (offset to the left)

	// Example view matrix (camera transformation)
	glm::vec3 cameraPosition(-1.0f, 0.0f, 0.0f); // Camera position
	glm::vec3 cameraTarget(0.0f, 0.0f, 0.0f);    // Camera target (where it's looking)
	glm::vec3 cameraUp(0.0f, 1.0f, 0.0f);        // Up vector

	this->viewMatrix = glm::lookAt(cameraPosition, cameraTarget, cameraUp);

	// Example projection matrix (parallel projection)
	float left = -1.0f;    // Left clipping plane
	float right = 1.0f;    // Right clipping plane
	float bottom = -1.0f;  // Bottom clipping plane
	float top = 1.0f;      // Top clipping plane
	float nearPlane = -1.0f; // Near clipping plane
	float farPlane = 1.0f;  // Far clipping plane

	this->projectionMatrix = glm::ortho(left, right, bottom, top, nearPlane, farPlane);

	// Set the default rotation matrix
	this->rotateTranslation = glm::mat4_cast(default_transl);

//	Theme 1:
//
//	Nodes: glm::vec3(0.8, 0.2, 0.2) (Light Red)
//	Rigid Elements : glm::vec3(0.1, 0.1, 0.8) (Dark Blue)
//	Spring Elements : glm::vec3(0.9, 0.6, 0.1) (Orange)
//	Mass Elements : glm::vec3(0.2, 0.7, 0.2) (Green)
//	Supports : glm::vec3(0.5, 0.5, 0.5) (Gray)
//	Background : glm::vec3(0.95, 0.95, 0.95) (Light Gray)
//
//	Theme 2 :
//
//	Nodes : glm::vec3(0.2, 0.6, 0.8) (Sky Blue)
//	Rigid Elements : glm::vec3(0.7, 0.3, 0.7) (Purple)
//	Spring Elements : glm::vec3(0.95, 0.8, 0.3) (Light Yellow)
//	Mass Elements : glm::vec3(0.3, 0.8, 0.4) (Light Green)
//	Supports : glm::vec3(0.4, 0.4, 0.4) (Dark Gray)
//	Background : glm::vec3(0.1, 0.1, 0.1) (Dark Black)
//
//	Theme 3 :
//
//	Nodes : glm::vec3(1.0, 0.0, 0.0) (Red)
//	Rigid Elements : glm::vec3(0.0, 0.0, 1.0) (Blue)
//	Spring Elements : glm::vec3(1.0, 1.0, 0.0) (Yellow)
//	Mass Elements : glm::vec3(0.0, 1.0, 0.0) (Green)
//	Supports : glm::vec3(0.5, 0.5, 0.5) (Gray)
//	Background : glm::vec3(0.9, 0.9, 0.9) (Light Gray)

	/*
	// Initialize the color theme
	geom_colors.background_color = glm::vec3(0.62f, 0.62f, 0.62f); 
	geom_colors.node_color = glm::vec3(0.0f, 0.0f, 0.4f); 
	geom_colors.selection_color = glm::vec3(0.8039f, 0.3608f, 0.3608f); 

	geom_colors.line_color = glm::vec3(0.0f, 0.2f, 0.6f); 
	geom_colors.constraint_color = glm::vec3(0.0f, 0.1f, 0.0f); 
	
	// Traingle mesh
	geom_colors.triangle_color = glm::vec3(0.82f, 0.77f, 0.92f); 
	*/
}


bool geom_parameters::isPointInsideRectangle(const glm::vec2& rect_cpt1, const glm::vec2& rect_cpt2, const glm::vec2& pt)
{
	return (pt.x >= std::min(rect_cpt1.x, rect_cpt2.x) &&
		pt.x <= std::max(rect_cpt1.x, rect_cpt2.x) &&
		pt.y >= std::min(rect_cpt1.y, rect_cpt2.y) &&
		pt.y <= std::max(rect_cpt1.y, rect_cpt2.y));
}

glm::vec2 geom_parameters::linear_interpolation(const glm::vec2& pt1, const glm::vec2& pt2, const double& param_t)
{
	return glm::vec2(pt1.x * (1.0 - param_t) + (pt2.x * param_t),
					 pt1.y * (1.0 - param_t) + (pt2.y * param_t));

}

glm::vec3 geom_parameters::linear_interpolation3d(const glm::vec3& pt1, const glm::vec3& pt2, const double& param_t)
{
	return glm::vec3(pt1.x * (1.0 - param_t) + (pt2.x * param_t),
					 pt1.y * (1.0 - param_t) + (pt2.y * param_t),
					 pt1.z * (1.0 - param_t) + (pt2.z * param_t));

}

void geom_parameters::copyNodenumberlistToCharArray(const std::vector<int>& vec, char* charArray, size_t bufferSize)
{
	// Use std::ostringstream to build the comma-separated string
	std::ostringstream oss;
	for (size_t i = 0; i < vec.size(); ++i)
	{
		if (i > 0)
		{
			oss << ", "; // Add a comma and space for each element except the first one
		}
		oss << vec[i];
	}

	// Copy the resulting string to the char array
	std::string resultString = oss.str();

	if (resultString.size() + 1 > bufferSize)
	{
		// Truncate 15 character
		resultString.erase(bufferSize - 16);
		resultString += "..exceeds limit";
	}

	strncpy_s(charArray, bufferSize, resultString.c_str(), _TRUNCATE);

}


glm::vec3 geom_parameters::get_standard_color(int color_index)
{
	// Red, Green, Blue, Yellow, Magenta, Cyan, Orange, Purple, Lime, Pink
	static const std::vector<glm::vec3> colorSet = {
			glm::vec3(1.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f),
			glm::vec3(0.0f, 0.0f, 1.0f),
			glm::vec3(1.0f, 1.0f, 0.0f),
			glm::vec3(1.0f, 0.0f, 1.0f),
			glm::vec3(0.0f, 1.0f, 1.0f),
			glm::vec3(1.0f, 0.5f, 0.0f),
			glm::vec3(0.5f, 0.0f, 1.0f),
			glm::vec3(0.5f, 1.0f, 0.0f),
			glm::vec3(1.0f, 0.0f, 0.5f)
	};

	int index = color_index % colorSet.size();
	return colorSet[index];
}


glm::vec3 geom_parameters::findGeometricCenter(const std::vector<glm::vec3>& all_pts)
{
	// Function returns the geometric center of the nodes
		// Initialize the sum with zero
	glm::vec3 sum(0);

	// Sum the points
	for (const auto& pt : all_pts)
	{
		sum += pt;
	}
	return sum / static_cast<float>(all_pts.size());
}

glm::vec3 geom_parameters::findGeometricCenter(const glm::vec3& pt1, const glm::vec3& pt2, const glm::vec3& pt3, const glm::vec3& pt4)
{
	// Function returns the geometric center of four nodes
	glm::vec3 sum(0);

	sum += pt1;
	sum += pt2;
	sum += pt3;
	sum += pt4;

	return sum / 4.0f;
}


std::pair<glm::vec3, glm::vec3> geom_parameters::findMinMaxXY(const std::vector<glm::vec3>& all_pts)
{
	if (static_cast<int>(all_pts.size()) < 1)
	{
		// Null input
		return {glm::vec3(0),glm::vec3(0)};
	}

	// Initialize min and max values to first node in map
	glm::vec3 firstNode = all_pts[0];
	glm::vec3 minXY = glm::vec3(firstNode.x, firstNode.y,firstNode.z);
	glm::vec3 maxXY = minXY;

	// Loop through all nodes in map and update min and max values
	for (const auto& pt : all_pts)
	{
		// Minimum
		if (pt.x < minXY.x)
		{
			minXY.x = pt.x;
		}
		if (pt.y < minXY.y)
		{
			minXY.y = pt.y;
		}
		if (pt.z < minXY.z)
		{
			minXY.z = pt.z;
		}

		// Maximum
		if (pt.x > maxXY.x)
		{
			maxXY.x = pt.x;
		}
		if (pt.y > maxXY.y)
		{
			maxXY.y = pt.y;
		}
		if (pt.z > maxXY.z)
		{
			maxXY.z = pt.z;
		}
	}

	// Return pair of min and max values
	return { minXY, maxXY };
}

glm::vec3 geom_parameters::getHeatMapColor(float value)
{
	float hsl_h = value * 240;
	const int alpha_i = 120;
	const float hsl_s = 1.0;
	const float hsl_l = 0.5;

	double r = 0;
	double g = 0;
	double b = 0;


	if (hsl_s == 0)
	{
		r = g = b = (hsl_l * 255);
	}
	else
	{
		double v1, v2;
		double hue = hsl_h / 360;

		v2 = (hsl_l < 0.5) ? (hsl_l * (1 + hsl_s)) : ((hsl_l + hsl_s) - (hsl_l * hsl_s));
		v1 = (2 * hsl_l) - v2;

		r = (255 * HueToRGB(v1, v2, hue + (1.0f / 3)));
		g = (255 * HueToRGB(v1, v2, hue));
		b = (255 * HueToRGB(v1, v2, hue - (1.0f / 3)));
	}

	glm::vec3 color = glm::vec3(r / 255.0f, g / 255.0f, b / 255.0f);
	return color;
}

double geom_parameters::HueToRGB(double v1, double v2, double vH)
{
	if (vH < 0)
		vH += 1;

	if (vH > 1)
		vH -= 1;

	if ((6 * vH) < 1)
		return (v1 + (v2 - v1) * 6 * vH);

	if ((2 * vH) < 1)
		return v2;

	if ((3 * vH) < 2)
		return (v1 + (v2 - v1) * ((2.0f / 3) - vH) * 6);

	return v1;
}


glm::vec3 geom_parameters::getContourColor_d(float value)
{
	// return the contour color based on the value (0 to 1)
	glm::vec3 color;
	float r, g, b;

	// Rainbow color map
	float hue = value * 5.0f; // Scale the value to the range of 0 to 5
	float c = 1.0f;
	float x = c * (1.0f - glm::abs(glm::mod(hue / 2.0f, 1.0f) - 1.0f));
	float m = 0.0f;

	if (hue >= 0 && hue < 1) {
		r = c;
		g = x;
		b = m;
	}
	else if (hue >= 1 && hue < 2) {
		r = x;
		g = c;
		b = m;
	}
	else if (hue >= 2 && hue < 3) {
		r = m;
		g = c;
		b = x;
	}
	else if (hue >= 3 && hue < 4) {
		r = m;
		g = x;
		b = c;
	}
	else {
		r = x;
		g = m;
		b = c;
	}

	color = glm::vec3(r, g, b);
	return color;
}



double geom_parameters::get_triangle_area(const glm::vec2& pt1, const glm::vec2& pt2, const glm::vec2& pt3)
{
	double x1 = static_cast<double>(pt1.x);
	double y1 = static_cast<double>(pt1.y);
	double x2 = static_cast<double>(pt2.x);
	double y2 = static_cast<double>(pt2.y);
	double x3 = static_cast<double>(pt3.x);
	double y3 = static_cast<double>(pt3.y);

	// Shoelace formula
	double area = 0.5 * (x1 * (y2 - y3) + x2 * (y3 - y1) + x3 * (y1 - y2));

	return area;
}

double geom_parameters::get_line_length(const glm::vec2& pt1, const glm::vec2& pt2)
{
	// Length of line
	double length = std::sqrt(std::pow(pt1.x - pt2.x, 2) + std::pow(pt1.y - pt2.y, 2));

	return length;
}


glm::vec2 geom_parameters::calculateCatmullRomPoint(const std::vector<glm::vec2>& controlPoints, float t)
{
	// Function to calculate a point on a Catmull-Rom spline
	int n = static_cast<int>(controlPoints.size()) - 1;
	int i = static_cast<int>(t * n);
	t = t * n - i;

	glm::vec2 p0 = i > 0 ? controlPoints[i - 1] : controlPoints[0];
	glm::vec2 p1 = controlPoints[i];
	glm::vec2 p2 = controlPoints[i + 1];
	glm::vec2 p3 = (i + 2 < n) ? controlPoints[i + 2] : controlPoints[n];

	return 0.5f * (
		(-p0 + 3.0f * p1 - 3.0f * p2 + p3) * (t * t * t) +
		(2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3) * (t * t) +
		(-p0 + p2) * t +
		2.0f * p1
		);
}

double geom_parameters::get_lerp(const double& max_value, const double& min_value, const double& param_t)
{
	// Linear interpolation based on paramter t (between max and min value)
	return ((1.0f - param_t) * min_value) + (max_value * param_t);
}

double geom_parameters::get_invlerp(const double& max_value, const double& min_value, const double& value)
{
	// Returns the value normalized between 0 and 1 (based on max & min)
	return ((value - min_value) / (max_value - min_value));
}


double geom_parameters::get_remap(const double& max_value, const double& min_value, const double& limit_max,
	const double& limit_min, const double& value)
{
	// Remap the value between limit_max and limit min
	double param_t = get_invlerp(max_value, min_value, value);
	return get_lerp(limit_max, limit_min, param_t);
}


double geom_parameters::roundToSixDigits(const double& number)
{
	return std::round(number * 1e6) / 1e6;
}

glm::vec3 geom_parameters::get_face_normal(const glm::vec3& pt1, const glm::vec3& pt2, const glm::vec3& pt3)
{
	// Calculate two vectors representing two sides of the triangle
	glm::vec3 edge1 = pt2 - pt1;
	glm::vec3 edge2 = pt3 - pt1;

	// Calculate the cross product of the two edges
	glm::vec3 normal = glm::cross(edge1, edge2);

	// Check if the normal is pointing in the opposite direction of the camera (negative z)
	// If so, reverse the order of points to ensure counter-clockwise ordering
	if (normal.z < 0.0f)
	{
		return glm::normalize(-normal); // Return the reversed normal (negated)
	}
	else
	{
		return glm::normalize(normal); // Return the normal as is
	}
}


//// Stop watch
//void Stopwatch::reset_time()
//{
//	m_startTime = std::chrono::high_resolution_clock::now();
//}
//
//double Stopwatch::current_elapsed() const
//{
//	return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - m_startTime).count() / 1000.0;
//}