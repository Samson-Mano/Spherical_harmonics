#version 330 core

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;


uniform mat4 panTranslation;
uniform mat4 rotateTranslation;
uniform float zoomscale;

uniform float normalized_deflscale; // Sine cycle from animation (-1 to 1)
uniform float deflscale; // Deflection scale value = normalized_deflscale (varies 0 to 1) * max deformation
uniform float transparency = 1.0f;
uniform float geom_scale;

layout(location = 0) in vec3 node_position;
layout(location = 1) in vec3 vertexColor;

out vec4 v_Color;

void main()
{
	// apply zoom scaling and Rotation to model matrix
	mat4 scalingMatrix = mat4(1.0)*zoomscale;
	scalingMatrix[3][3] = 1.0f;
	mat4 scaledModelMatrix =  rotateTranslation * scalingMatrix * modelMatrix;
	
	// Declare variable to store final node center
	vec4 finalPosition;
	vec3 final_vertexColor;


	// apply Translation to the final position 
	// finalPosition =  scaledModelMatrix * viewMatrix * vec4(node_position,1.0f) * panTranslation;
	finalPosition =  scaledModelMatrix * vec4(node_position,1.0f) * panTranslation;

	// Final position passed to fragment shader
	// gl_Position = projectionMatrix * finalPosition;
	gl_Position = finalPosition;

	v_Color = vec4(vertexColor,transparency);

}