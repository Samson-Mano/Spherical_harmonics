#version 330 core

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

uniform float normalized_deflscale; // Sine cycle from animation (-1 to 1)
uniform float deflscale; // Deflection scale value = normalized_deflscale (varies 0 to 1) * max display deformation (controlled by user)
uniform float transparency = 1.0f;
uniform float geom_scale;

layout(location = 0) in vec3 node_position;
layout(location = 1) in vec3 node_defl;
layout(location = 2) in vec3 otherpt1_position;
layout(location = 3) in vec3 otherpt1_defl;
layout(location = 4) in vec3 otherpt2_position;
layout(location = 5) in vec3 otherpt2_defl;

layout(location = 6) in float defl_length; // Deflection length Normalized to 1.0

out float v_defl_length;
out float v_normalized_deflscale;
out vec3 v_Normal;

vec3 get_face_normal(vec3 pt1, vec3 pt2, vec3 pt3)
{
    // Calculate two vectors representing two sides of the triangle
    vec3 edge1 = pt2 - pt1;
    vec3 edge2 = pt3 - pt1;

    // Calculate the cross product of the two edges
    vec3 normal = cross(edge1, edge2);

    // Return the normalized normal
    return normalize(normal);
}


void main()
{

	// Scale based on model
	float node_circe_radii = 0.005f;
	float defl_ratio = deflscale * (node_circe_radii/ geom_scale);

	// Scale the node deflection point
	vec3 nodedefl_position = vec3(node_position.x + (node_defl.x * defl_ratio), 
								node_position.y + (node_defl.y * defl_ratio),
								node_position.z + (node_defl.z * defl_ratio));

	// Scale the triangle's other pt1 deflection point
	vec3 otherpt1defl_position = vec3(otherpt1_position.x + (otherpt1_defl.x * defl_ratio), 
								otherpt1_position.y + (otherpt1_defl.y * defl_ratio),
								otherpt1_position.z + (otherpt1_defl.z * defl_ratio));


	// Scale the triangle's other pt2 deflection point
	vec3 otherpt2defl_position = vec3(otherpt2_position.x + (otherpt2_defl.x * defl_ratio), 
								otherpt2_position.y + (otherpt2_defl.y * defl_ratio),
								otherpt2_position.z + (otherpt2_defl.z * defl_ratio));


    // Calculate the scaled node normal
	vec3 node_normal = get_face_normal(nodedefl_position, otherpt1defl_position, otherpt2defl_position);

		
	// Vertex Normal transformed using only model-view matrix (not projection)
    v_Normal = normalize(transpose(inverse(mat3(viewMatrix * modelMatrix))) * node_normal);


	//________________________________________________________________________________________________________________
	// apply Translation to the node position
	gl_Position =  projectionMatrix * viewMatrix * modelMatrix * vec4(nodedefl_position,1.0f);


	// Color
	v_defl_length = defl_length;
	v_normalized_deflscale = normalized_deflscale;


}