#version 330 core

uniform mat4 modelMatrix;
uniform mat4 panTranslation;
uniform mat4 rotateTranslation;
uniform float zoomscale;

uniform float transparency = 1.0f;
uniform vec3 ptColor;

layout(location = 0) in vec3 node_position;
layout(location = 1) in vec3 node_normal;

out vec4 v_Color;

// Lighting parameters
const vec3 lightDirection = normalize(vec3(0.0, 0.0, 1.0)); // Directional light
const vec3 ambientColor = vec3(0.2, 0.2, 0.2); // Ambient light color
const vec3 diffuseColor = vec3(0.8, 0.8, 0.8); // Diffuse light color
const vec3 specularColor = vec3(1.0, 1.0, 1.0); // Specular light color
const float shininess = 32.0; // Shininess coefficient

void main()
{
	// apply zoom scaling and Rotation to model matrix
	mat4 scalingMatrix = mat4(1.0)*zoomscale;
	scalingMatrix[3][3] = 1.0f;
	mat4 scaledModelMatrix =  rotateTranslation * scalingMatrix * modelMatrix;


	// Final position passed to fragment shader
	vec4 clipSpacePosition = scaledModelMatrix * vec4(node_position,1.0f) * panTranslation;
    gl_Position = clipSpacePosition;

    // Calculate normal in world space
    vec3 normal = normalize(mat3(transpose(inverse(scaledModelMatrix))) * node_normal);

    // Calculate light direction in world space
    vec3 lightDir = normalize(lightDirection);

    // Calculate ambient light component
    vec3 ambient = ambientColor * ptColor;

    // Calculate diffuse light component
    float diffuseIntensity = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diffuseColor * ptColor * diffuseIntensity;

    // Calculate specular light component
    vec3 viewDir = normalize(-vec3(clipSpacePosition));
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = specularColor * spec;

    // Combine all lighting components
    vec3 lighting = ambient + diffuse + specular;

    // Final color passed to fragment shader
    v_Color = vec4(lighting, transparency);

}