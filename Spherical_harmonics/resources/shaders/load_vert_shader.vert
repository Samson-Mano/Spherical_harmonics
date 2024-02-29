#version 330 core

uniform mat4 modelMatrix;
// uniform mat4 viewMatrix;
// uniform mat4 projectionMatrix;

uniform mat4 panTranslation;
uniform mat4 rotateTranslation;
uniform float zoomscale;
uniform float transparency = 1.0f;
uniform vec3 ptColor;

layout(location = 0) in vec3 loadend_position;
layout(location = 1) in vec3 load_center;
layout(location = 2) in vec3 vertexNormal;

out vec4 v_Color;
out vec3 v_Normal;
out vec3 v_FragPos;

void main()
{
	// apply zoom scaling and Rotation to model matrix
	mat4 scalingMatrix = mat4(1.0)*zoomscale;
	scalingMatrix[3][3] = 1.0f;
	mat4 scaledModelMatrix = rotateTranslation * scalingMatrix * modelMatrix;
	
	// Apply translation to load end position
	vec4 final_loadend_Position = scaledModelMatrix * vec4(loadend_position, 1.0f) * panTranslation;
	
	// Apply translation to the load center
	vec4 final_load_center = scaledModelMatrix * vec4(load_center, 1.0f) * panTranslation;

	v_Color = vec4(ptColor, transparency);
	
	// Scale the final quad position
	vec3 scaled_pt = vec3(final_loadend_Position.x - final_load_center.x, 
						  final_loadend_Position.y - final_load_center.y,
						  final_loadend_Position.z - final_load_center.z);

	// Final position passed to shader
	gl_Position = vec4(final_load_center.x + scaled_pt.x, 
						final_load_center.y + scaled_pt.y, 
						final_load_center.z + scaled_pt.z, 1.0f);

	v_FragPos = ((scaledModelMatrix* vec4(loadend_position, 1.0f)).xyz);

	// Pass the load normal
	vec4 surfNormal = (rotateTranslation * vec4(vertexNormal,1.0f));
    v_Normal = normalize(surfNormal.xyz);

}