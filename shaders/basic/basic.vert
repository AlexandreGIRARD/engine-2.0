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

uniform mat4 projection_view;
uniform mat4 model;

out vec2 uv;

void main()
{
    uv = texcoord_0;
    gl_Position = projection_view * vec4(pos, 1);
}