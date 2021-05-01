#version 450 core

layout (location = 0) in vec3 pos;

out vec3 world_pos;

void main()
{
    world_pos = pos;
    gl_Position = vec4(pos, 1.0);
}