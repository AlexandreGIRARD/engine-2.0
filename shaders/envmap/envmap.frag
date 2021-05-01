#version 450 core

const vec2 invAtan = vec2(0.1591, 0.3183);

layout (binding = 0) uniform sampler2D hdr_map;

in vec3 pos;

out vec4 color;

// from learnopengl
vec2 SampleSphericalMap(vec3 v)
{
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}

void main()
{		
    vec2 uv = SampleSphericalMap(normalize(os)); // make sure to normalize localPos
    vec3 color = texture(hdr_map, uv).rgb;
    
    color = vec4(color, 1.0);
}