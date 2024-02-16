#version 330 core

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;


uniform mat4 panTranslation;
uniform mat4 rotateTranslation;
uniform float zoomscale;

uniform float normalized_deflscale; // Sine cycle from animation (-1 to 1)
uniform float deflscale; // Deflection scale value = normalized_deflscale (varies 0 to 1) * max display deformation (controlled by user)
uniform float transparency = 1.0f;
uniform float geom_scale;

layout(location = 0) in vec3 node_position;
layout(location = 1) in vec3 node_defl;
layout(location = 2) in float defl_length; // Deflection length Normalized to 1.0

out float v_defl_length;
out float v_normalized_deflscale;
out float v_transparency;

void main()
{
	// apply zoom scaling and Rotation to model matrix
	mat4 scalingMatrix = mat4(1.0)*zoomscale;
	scalingMatrix[3][3] = 1.0f;
	mat4 scaledModelMatrix =  rotateTranslation * scalingMatrix * modelMatrix;
	
	// Declare variable to store final node center
	vec3 final_vertexColor;

	// Scale based on model
	float node_circe_radii = 0.005f;
	float defl_ratio = deflscale * (node_circe_radii/ geom_scale);

	// Scale the deflection point
	vec3 defl_position = vec3(node_position.x + (node_defl.x * defl_ratio), 
								node_position.y + (node_defl.y * defl_ratio),
								node_position.z + (node_defl.z * defl_ratio));

	// apply Translation to the node position
	gl_Position = scaledModelMatrix * vec4(defl_position,1.0f) * panTranslation;

	// Color
	v_defl_length = defl_length;
	v_normalized_deflscale = normalized_deflscale;
	v_transparency = transparency;

}