#version 330 core

uniform mat4 modelMatrix;
uniform mat4 panTranslation;
uniform mat4 rotateTranslation;
uniform float zoomscale;

uniform float transparency;
uniform vec3 ptColor;

layout(location = 0) in vec3 node_position;
layout(location = 1) in vec3 node_normal;

out vec3 v_Normal;
out vec3 v_Color;
out float v_transparency;
out vec3 v_FragPos;

void main()
{
	// apply zoom scaling and Rotation to model matrix
	mat4 scalingMatrix = mat4(1.0)*zoomscale;
	scalingMatrix[3][3] = 1.0f;
	mat4 scaledModelMatrix =  rotateTranslation * scalingMatrix * modelMatrix;


	// Final position passed to fragment shader
    gl_Position = scaledModelMatrix * vec4(node_position,1.0f) * panTranslation;

    //_______________________________________________________________________________________

	
	v_FragPos = (rotateTranslation * modelMatrix * vec4(node_position,1.0f)).xyz;

    // Send the vertex normal to the fragment shader
	//calculate normal in world coordinates
    vec4 surfNormal = (rotateTranslation * vec4(node_normal,1.0f));
    v_Normal = normalize(surfNormal.xyz);

    v_Color = ptColor;

	// mat3 normal_matrix = transpose(inverse(mat3(rotateTranslation)));
    // v_Normal = normalize( normal_matrix * node_normal);


	v_transparency = transparency;

}