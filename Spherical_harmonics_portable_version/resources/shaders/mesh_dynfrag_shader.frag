#version 330 core

in float v_defl_length;
in float v_normalized_deflscale;
in vec3 v_Normal;

out vec4 f_Color; // fragment's final color (out to the fragment shader)


vec3 unreal(vec3 x) 
{
    return x / (x + 0.155) * 1.019;
}


vec3 jetHeatmap(float value) 
{

    return clamp(vec3(1.5) - abs(4.0 * vec3(value) + vec3(-3, -2, -1)), vec3(0), vec3(1));
}


void main() 
{

    // Set the color    
    float interpolated_defl = v_defl_length *  v_normalized_deflscale; // varies between maximum of -1.0 to 1.0

    vec3 vertexColor = jetHeatmap(interpolated_defl);

    //_______________________________________________________________________________
    // View direction fixed at -z
    vec3 viewPos = vec3(0.0, 0.0, -1.0);

    // Light direction fixed at -z
    vec3 uniLightDir = vec3(0.0, 0.0, -1.0);

    // Halfway vector between view direction and light direction
    vec3 halfDir = normalize(viewPos + uniLightDir);

    // Color with slight mix to white for specular highlights
    vec3 specColor = mix(vertexColor, vec3(1.0, 1.0, 1.0), 0.1) * 1.5;
    vec3 diffColor = vertexColor;
    float shineness = 40.0;

    // Calculate specular and diffuse lighting
    float specular = pow(max(0.0, dot(halfDir, v_Normal)), shineness);
    float diffuse = max(0.0, dot(uniLightDir, v_Normal));
    float ambient = 0.05;
    
    // Final color with lighting
    vec3 finalColor = (diffuse + ambient) * diffColor + specular * specColor;

    // Tone mapping
    finalColor = unreal(finalColor);

    //___________________________________________________________________________________________________________

    f_Color = vec4(finalColor, 1.0f); // Set the final color
}
