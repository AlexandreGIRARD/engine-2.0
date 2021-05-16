#version 450 core

layout (binding = 0) uniform sampler2D frame_tex;
layout (binding = 1) uniform sampler2D factor_tex;
layout (binding = 2) uniform sampler2D blured_tex;

in vec2 frag_uv;

layout (location = 0) out vec3 frag_output;

void main()
{
    vec3 color   = texture(frame_tex, frag_uv).rgb;
    float factor = texture(factor_tex, frag_uv).r;
    vec3 blured  = texture(blured_tex, frag_uv).rgb;

    frag_output = mix(color, blured, factor);
}