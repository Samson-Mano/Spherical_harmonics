#pragma once
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/quaternion.hpp>>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <unordered_map>
#include "../geometry_store/geom_parameters.h"

class arcball_transformation
{
public:
	arcball_transformation();
	~arcball_transformation();
	void OnMouseDown(glm::vec2 mousePt);
	void OnMouseMove(glm::vec2 mousePt);
	void OnMouseUp(glm::vec2 mousePt);
	void setDefault(const int& viewType);
	glm::mat4 getRotationMatrix();// Rotation matrix

private:
	glm::vec3 startVector = glm::vec3(0); // Start vector
	glm::vec3 endVector = glm::vec3(0); // End vector
	glm::quat rotationQuaternion = glm::quat(1.0,0.0,0.0,0.0); // Rotation quaternion
	glm::quat lastTransformation = glm::quat(1.0, 0.0, 0.0, 0.0); // Last Transformation quaternion

	// glm::mat4 currentTransformation = glm::mat4(1.0f); // This Transformation

	glm::vec3 ProjectToSphere(glm::vec2 point, float radius);
};
