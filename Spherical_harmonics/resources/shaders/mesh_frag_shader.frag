#version 330 core

in vec3 v_Normal;
in vec3 v_Color;
in float v_transparency;
in vec3 v_FragPos;

out vec4 f_Color; // fragment's final color (out to the fragment shader)

const vec3 diffuseColor = vec3(0.9f, 0.9f, 0.9f); // Diffuse light color
const vec3 lightDir = vec3(200.0, 200.0, -2500.0); // Light Direction

// const vec3 lightDir = vec3(0.0, 0.0, -1.0); // Light Direction

void main()
{
    // diffuse Z Direction
    // Calculate diffuse light component
    float diffuseIntensity = 1.15f * (1.0f / length(lightDir))* max(dot(v_FragPos - lightDir,v_Normal),0.0);

    // float diffuseIntensity = max(dot(v_FragPos - lightDir,v_Normal),0.0);

    f_Color = vec4(diffuseColor * v_Color  , v_transparency)* diffuseIntensity;
}

