#version 450 core


layout(binding = 0) uniform sampler2D base_color;
layout(binding = 1) uniform sampler2D metallic_roughness;
layout(binding = 2) uniform sampler2D normal;

in vec2 uv;

out vec4 color;

void main()
{
    color = texture(base_color, vec2(uv.x,1.-uv.y));
}