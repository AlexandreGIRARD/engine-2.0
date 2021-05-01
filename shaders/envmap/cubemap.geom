#version 450 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 18) out;

uniform mat4 projection;
uniform mat4 views[6];

in vec3 world_pos[];

out vec3 pos;

void main()
{
    for (int i = 0; i < 6; i++)
    {
        gl_Layer = i;
        for (int j = 0; j < 3; j++)
        {
            pos = world_pos[j];
            gl_Position = projection * views[i] * vec4(world_pos[j], 1.0);
            EmitVertex();
        }
        EndPrimitive();
    }
}