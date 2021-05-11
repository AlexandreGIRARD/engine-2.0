#version 450 core

layout (binding = 0) uniform sampler2D frame_tex;
uniform float threshold;
uniform float strength;

in vec2 frag_uv;

layout (location = 0) out vec4 frag_output;

void main()
{
    vec3 color = texture(frame_tex, frag_uv).rgb;
    float brightness = dot(color, vec3(0.2126, 0.7152, 0.0722));
    if (brightness >= threshold)
        frag_output = vec4(strength * color, 1.0);
    else
        frag_output = vec4(0.0, 0.0, 0.0, 1.0);
}