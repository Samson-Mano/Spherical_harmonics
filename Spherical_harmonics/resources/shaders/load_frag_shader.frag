#version 330 core

in vec4 v_Color;
in vec3 v_Normal;
in vec3 v_FragPos;

out vec4 f_Color; // fragment's final color (out to the fragment shader)

const vec3 lightDir = vec3(200.0, 200.0, -2500.0); // Light Direction

void main()
{

	float diffuseIntensity = 1.15f * (1.0f / length(lightDir))* max(dot(v_FragPos - lightDir,v_Normal),0.0);
	
	f_Color = v_Color * diffuseIntensity;
}