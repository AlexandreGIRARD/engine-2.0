#version 450 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;
layout (location = 3) in vec3 tangent;

uniform mat4 view_proj;
uniform mat4 model;

void main()
{
    gl_Position = view_proj * model * vec4(pos, 1);
}