#version 450 core

const float weights[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

layout (binding = 0) uniform sampler2D to_blur_tex;
uniform int is_horizontal;
uniform int mip_level;

in vec2 frag_uv;

layout (location = 0) out vec4 frag_output;

void main()
{
    vec3 color = textureLod(to_blur_tex, frag_uv, mip_level).rgb * weights[0];
    ivec2 offset = ivec2(1, 0);
    if (is_horizontal == 0)
        offset = ivec2(0, 1);
    for (int i = 1; i < 5; i++)
    {
        color += textureLodOffset(to_blur_tex, frag_uv, mip_level, offset *  i).rgb * weights[i];
        color += textureLodOffset(to_blur_tex, frag_uv, mip_level, offset * -i).rgb * weights[i];
    }

    frag_output = vec4(color, 1.0);
}