#version 330 core

uniform mat4 modelMatrix;
uniform mat4 panTranslation;
uniform mat4 rotateTranslation;
uniform float zoomscale;

uniform float normalized_deflscale; // Sine cycle from animation (-1 to 1)
uniform float deflscale; // Deflection scale value = normalized_deflscale (varies 0 to 1) * max deformation
uniform float transparency = 1.0f;
uniform float geom_scale;

layout(location = 0) in vec3 node_position;
layout(location = 1) in vec3 node_defl;
layout(location = 2) in vec3 vertexColor;

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


	// Scale based on model
	float node_circe_radii = 0.005f;
	float defl_ratio = deflscale * (node_circe_radii/ geom_scale);

	// Scale the deflection point
	vec3 defl_position = vec3(node_position.x + (node_defl.x * defl_ratio), 
								node_position.y + (node_defl.y * defl_ratio),
								node_position.z + (node_defl.z * defl_ratio));

	// apply Translation to the node position
	finalPosition = scaledModelMatrix * vec4(defl_position,1.0f) * panTranslation;

	// Update the color based on cycle time
	final_vertexColor = vec3((0.5f*(1.0f-normalized_deflscale)+(vertexColor.x*normalized_deflscale)),
								(0.0f*(1.0f-normalized_deflscale)+(vertexColor.y*normalized_deflscale)),
								(1.0f*(1.0f-normalized_deflscale)+(vertexColor.z*normalized_deflscale)));



	// Final position passed to fragment shader
	// gl_Position = projectionMatrix * finalPosition;
	gl_Position = finalPosition;

	v_Color = vec4(final_vertexColor,transparency);

}