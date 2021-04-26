#version 450 core

struct fragment_info
{
    vec4 position;
    vec4 base_color;
    vec3 emissive;
    vec3 normal;
    float roughness;
    float metallic;
    float occlusion;
};

// G-Buffer textures
layout (binding = 0) uniform sampler2D position_tex;
layout (binding = 1) uniform sampler2D base_color_tex;
layout (binding = 2) uniform sampler2D orm_tex;
layout (binding = 3) uniform sampler2D emissive_tex;
layout (binding = 4) uniform sampler2D normal_tex;

// SSAO texture

// Shadow Map texture

// Lights

// IBL

in vec2 frag_uv;

// Output Attachment
layout (location = 0) out vec4 deferred_output;

void main()
{
    deferred_output = texture(normal_tex, frag_uv).rgba;
}