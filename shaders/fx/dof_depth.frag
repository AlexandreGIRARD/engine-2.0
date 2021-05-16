#version 450 core

layout (binding = 0) uniform sampler2D position_tex;
uniform float aperture;
uniform float focal_length;
uniform float z_focus;
uniform float z_range;
uniform mat4 view;

in vec2 frag_uv;

layout (location = 0) out float frag_output;

void main()
{
    vec4 pos = texture(position_tex, frag_uv);
    // Background -> full blur
    if (pos == vec4(0))
    {
        frag_output = 1.f;
        return;
    }
    float z = - (view * pos).z;
    float factor = abs((aperture * focal_length * (z_focus - z)) / (z_focus * (z - focal_length))); // More correct Circle of Confusion equation
    frag_output = clamp(factor, 0.f, 1.f);
}