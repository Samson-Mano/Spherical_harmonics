#version 330 core

in float v_defl_length;
in float v_normalized_deflscale;
in float v_transparency;
in vec3 v_Normal;
in vec3 v_FragPos;

out vec4 f_Color; // fragment's final color (out to the fragment shader)

const vec3 lightDir = vec3(200.0, 200.0, -2500.0); // Light Direction

vec3 jetHeatmap(float value) 
{

    return clamp(vec3(1.5) - abs(4.0 * vec3(value) + vec3(-3, -2, -1)), vec3(0), vec3(1));
}


void main() 
{
    // diffuse Z Direction
    // Calculate diffuse light component
    float diffuseIntensity =  1.15f * (1.0f / length(lightDir))* max(dot(v_FragPos - lightDir,v_Normal ),0.0);

    //___________________________________________________________________________________________________________

    float interpolated_defl = v_defl_length *  v_normalized_deflscale; // varies between maximum of -1.0 to 1.0

    vec3 vertexColor = jetHeatmap(interpolated_defl);

    f_Color = vec4(vertexColor, v_transparency) * diffuseIntensity; // Set the final color
}
