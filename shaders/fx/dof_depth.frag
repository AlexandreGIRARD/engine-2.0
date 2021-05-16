#version 450 core

layout (binding = 0) uniform sampler2D position_tex;
uniform float diameter;
uniform float focal_length;
uniform float z_focus;
uniform float z_range;
uniform mat4 view;

in vec2 frag_uv;

layout (location = 0) out float frag_output;

void main()
{
    float z = - (view * texture(position_tex, frag_uv)).z;
    float factor = abs((z_focus - z) / z_range);
    frag_output = factor;
}