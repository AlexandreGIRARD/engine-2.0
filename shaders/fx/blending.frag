#version 450 core

#define BLUR_LEVELS 4

layout (binding = 0) uniform sampler2D frame_tex;
uniform sampler2D bloom_tex[BLUR_LEVELS];

in vec2 frag_uv;

layout (location = 0) out vec4 frag_output;

void main()
{
    vec3 frame_color = texture(frame_tex, frag_uv).rgb;
    vec3 bloom_color = vec3(0);
    
    // Added bloom information over 4 mip levels
    for (int level = 0; level < 4; level++)
        bloom_color += texture(bloom_tex[level], frag_uv).rgb;

    frag_output = vec4(frame_color + bloom_color, 1.0);
}