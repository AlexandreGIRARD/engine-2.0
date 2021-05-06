#version 450 core

const int kernel_size = 64;

layout (binding = 0) uniform sampler2D position_tex;
layout (binding = 1) uniform sampler2D normal_tex;
layout (binding = 1) uniform sampler2D noise_tex;

uniform mat4 projection;
uniform mat4 view;
uniform vec2 noise_scale;

uniform vec3[kernel_size] samples;
uniform float radius;
uniform float power;

in vec2 frag_uv;

layout (location = 0) out float ambient_occlusion;

vec3 to_view_space(vec3 vect)
{
    return vec3(view * vec4(vect, 1.0));
}

void main()
{
    // To view space gbuffer informations
    vec3 origin = to_view_space(texture(position_tex, frag_uv).xyz);
    vec3 normal = normalize(to_view_space(texture(normal_tex, frag_uv).xyz));
    vec3 rvec   = texture(noise_tex, frag_uv * noise_scale).xyz;

    // Gram-Schimdt
    vec3 tangent = normalize(rvec - normal * dot(rvec, normal));
    vec3 bitangent = normalize(cross(normal, tangent));
    mat3 TBN = mat3(tangent, bitangent, normal);

    float occlusion = 0.0;
    for (int i = 0; i < kernel_size; i++) 
    {
        // get sample position:
        vec3 _sample = (TBN * samples[i]) * radius + origin;
        
        // project sample position:
        vec4 offset = vec4(_sample, 1.0);
        offset = projection * offset;
        offset.xy /= offset.w;
        offset.xy = offset.xy * 0.5 + 0.5;
        
        // get sample depth:
        float depth = to_view_space(texture(position_tex, offset.xy).xyz).z;
        
        // range check & accumulate:
        float check= smoothstep(0.0, 1.0, radius / abs(origin.z - depth));
        occlusion += (depth >= _sample.z + 0.025 ? 1.0 : 0.0) * check;
    }

    ambient_occlusion = 1.0 - (occlusion / kernel_size);
}