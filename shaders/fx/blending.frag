#version 450 core

layout (binding = 0) uniform sampler2D frame_tex;
layout (binding = 1) uniform sampler2D bloom_tex;

in vec2 frag_uv;

out vec4 frag_output;

void main()
{
    vec3 frame_color = texture(frame_tex, frag_uv).rgb;
    vec3 bloom_color = texture(bloom_tex, frag_uv).rgb;

    frag_output = vec4(frame_color + bloom_color, 1.0);
}