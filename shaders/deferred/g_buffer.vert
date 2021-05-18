#version 450 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec3 tangent;
layout (location = 3) in vec2 texcoord_0;
layout (location = 4) in vec2 texcoord_1;
layout (location = 5) in vec2 texcoord_2;
layout (location = 6) in vec2 texcoord_3;
layout (location = 7) in vec2 texcoord_4;
layout (location = 8) in vec2 texcoord_5;

out VS_OUT
{
    vec4 pos;
    vec3 normal;
    vec2 uv;
    mat3 TBN;
    vec2[6] tex_coords;
} vs_out;

uniform mat4 projection_view;
uniform mat4 model;
uniform mat4 normal_matrix;

void main()
{
    vs_out.normal = normalize(vec3(normal_matrix * vec4(normal, 1.0)));
    vs_out.pos = model * vec4(pos, 1.0);
    vs_out.tex_coords[0] = texcoord_0;
    vs_out.tex_coords[1] = texcoord_1;
    vs_out.tex_coords[2] = texcoord_2;
    vs_out.tex_coords[3] = texcoord_3;
    vs_out.tex_coords[4] = texcoord_4;
    vs_out.tex_coords[5] = texcoord_5;

    // TBN
    vec3 T = normalize(vec3(model * vec4(tangent, 0.0)));
    vec3 N = normalize(vec3(model * vec4(normal, 0.0)));
    vec3 B = normalize(cross(vs_out.normal, T));

    vs_out.TBN = mat3(T, B, N);

    gl_Position = projection_view * model * vec4(pos, 1.0);
}