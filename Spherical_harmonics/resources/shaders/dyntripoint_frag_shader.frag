#version 330 core

in float v_defl_length;
in float v_normalized_deflscale;
in float v_transparency;

out vec4 f_Color; // fragment's final color (out to the fragment shader)


vec3 jetHeatmap(float value) 
{

    return clamp(vec3(1.5) - abs(4.0 * vec3(value) + vec3(-3, -2, -1)), vec3(0), vec3(1));
}


void main() 
{


    //_____________________________________________________________________

    float interpolated_defl = v_defl_length *  v_normalized_deflscale; // varies between maximum of -1.0 to 1.0

    vec3 vertexColor = jetHeatmap(interpolated_defl);

    f_Color = vec4(vertexColor, v_transparency); // Set the final color
}
