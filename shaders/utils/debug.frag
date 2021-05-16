#version 450 core

#define G_POSITION        0
#define G_COLOR           1
#define G_NORMAL          2
#define G_METALLIC        3
#define G_ROUGHNESS       4
#define G_EMISSIVE        5
#define G_AO              6
#define AMBIENT_OCCLUSION 7
#define DOF               8

layout (binding = 0) uniform sampler2D debug_tex;

uniform int mode;

in vec2 frag_uv;

layout (location = 0) out vec4 debug_output;

void main()
{
    vec4 info = texture(debug_tex, frag_uv).rgba;
    debug_output = info;
    if (mode == G_METALLIC)
        debug_output = vec4(info.b);
    else if (mode == G_ROUGHNESS)
        debug_output = vec4(info.g);
    else if (mode == G_AO)
        debug_output = vec4(info.r);
    else if (mode == AMBIENT_OCCLUSION || mode == DOF)
        debug_output = vec4(info.r);
}