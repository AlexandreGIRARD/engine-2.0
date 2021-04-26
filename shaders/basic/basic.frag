#version 450 core

layout(binding = 0) uniform sampler2D base_color;
layout(binding = 1) uniform sampler2D metallic_roughness;
layout(binding = 2) uniform sampler2D normal;
layout(binding = 3) uniform sampler2D emissive;
layout(binding = 4) uniform sampler2D ambient_occlusion;

in vec2 uv;

out vec4 color;

void main()
{
    vec4 albedo = texture(base_color, uv);
    if (albedo.a == 0)
        discard;
    color = albedo;//vec2(uv.x,1.-uv.y));
}