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
} vs_out;

uniform mat4 projection_view;
uniform mat4 model;
uniform mat4 normal_matrix;

void main()
{
    vs_out.pos = model * vec4(pos, 1.0);
    vs_out.uv = texcoord_0;

    // TBN
    vec3 T = vec3(normal_matrix * vec4(tangent, 1.0));
    vs_out.normal = vec3(normal_matrix * vec4(normal, 1.0));
    vec3 B = cross(vs_out.normal, T);

    vs_out.TBN = mat3(T, B, vs_out.normal);

    gl_Position = projection_view * model * vec4(pos, 1.0);
}