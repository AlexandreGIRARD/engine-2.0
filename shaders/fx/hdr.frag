#version 450 core

#define BASIC 0
#define FILMIC 1

layout (binding = 0) uniform sampler2D frame_tex;
uniform float exposure;
uniform float gamma;
uniform int algorithm;

in vec2 frag_uv;

layout (location = 0) out vec4 frag_output;

// https://github.com/ampas/aces-dev
// https://knarkowicz.wordpress.com/2016/01/06/aces-filmic-tone-mapping-curve/
vec3 ACES_Filmic(vec3 x)
{
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    return clamp((x*(a*x+b))/(x*(c*x+d)+e), 0.0, 1.0);
}

void main()
{
    vec3 color = texture(frame_tex, frag_uv).rgb;
    if (algorithm == BASIC)
        color = color / (color + vec3(1.0));
    else if (algorithm == FILMIC)
        color = ACES_Filmic(color);
    
    // Exposure
    color = vec3(1.0) - exp(-color * exposure);

    // Gamma correction
    color = pow(color, vec3(1.0 / gamma)); 
    frag_output = vec4(color, 1.0);
}