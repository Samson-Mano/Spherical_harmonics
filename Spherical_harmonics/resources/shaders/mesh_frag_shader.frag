
#version 330 core

in vec3 vertNormal;
in vec3 v_Color;

out vec4 f_Color; // Final color output

vec3 unreal(vec3 x) 
{
    return x / (x + 0.155) * 1.019;
}

void main() 
{

    // View direction fixed at -z
    vec3 viewPos = vec3(0.0, 0.0, -1.0);

    // Light direction fixed at -z
    vec3 uniLightDir = vec3(0.0, 0.0, -1.0);

    // Halfway vector between view direction and light direction
    vec3 halfDir = normalize(viewPos + uniLightDir);

    // Color with slight mix to white for specular highlights
    vec3 specColor = mix(v_Color, vec3(1.0, 1.0, 1.0), 0.1) * 1.5;
    vec3 diffColor = v_Color;
    float shineness = 40.0;

    // Calculate specular and diffuse lighting
    float specular = pow(max(0.0, dot(halfDir, vertNormal)), shineness);
    float diffuse = max(0.0, dot(uniLightDir, vertNormal));
    float ambient = 0.05;
    
    // Final color with lighting
    vec3 finalColor = (diffuse + ambient) * diffColor + specular * specColor;

    // Tone mapping
    finalColor = unreal(finalColor);
    f_Color = vec4(finalColor, 1.0f ); // v_Transparency
}





