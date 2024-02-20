#version 330 core

in vec3 v_Normal;
in vec3 v_Color;
in float v_transparency;

out vec4 f_Color; // fragment's final color (out to the fragment shader)

const vec3 viewDir = vec3(-200.0, -200.0, 2500.0); // View Direction

void main()
{
    // diffuse Z Direction
    float diff = 1.15f * (1.0f / length(viewDir)) * max(dot(v_Normal, viewDir), 0.0);

    f_Color = vec4(v_Color, v_transparency) * diff;
}

