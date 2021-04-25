#version 450 core

// Textures
layout (binding = 0) uniform sampler2D base_color_tex;
layout (binding = 1) uniform sampler2D metallic_roughness_tex;
layout (binding = 2) uniform sampler2D normal_tex;
layout (binding = 3) uniform sampler2D emissive_tex;
layout (binding = 4) uniform sampler2D occlusion_tex;

// Factors
uniform vec4   base_color_factor;
uniform vec3   emissive_factor;
uniform double metallic_factor;
uniform double roughness_factor;
uniform double occlusion_factor;

// Textures informations
uniform int has_base_color_tex;
uniform int has_metallic_roughness_tex;
uniform int has_normal_tex;
uniform int has_emissive_tex;
uniform int has_occlusion_tex;

// Vertex Shader Outputs
in VS_OUT
{
    vec4 pos;
    vec3 normal;
    vec2 uv;
    mat3 TBN;
} fs_in;

// Attachments
layout (location = 0) out vec4 position;
layout (location = 1) out vec4 base_color;
layout (location = 2) out vec3 orm;
layout (location = 3) out vec3 emissive;
layout (location = 4) out vec3 normal;

void main()
{
    // Base Color
    if (has_base_color_tex)
        base_color = texture(base_color_tex, fs_in.uv).rgba;
    else
        base_color = base_color_factor;
    
    // Occlusion
    if (has_occlusion_tex)
        orm.r = texture(occlusion_tex, fs_in.uv).r;
    else
        orm.r = occlusion_factor;
    
    // Metallic Roughness
    if (has_metallic_roughness_tex)
    {
        orm.g = texture(metallic_roughness_tex, fs_in.uv).g;
        orm.b = texture(metallic_roughness_tex, fs_in.uv).b;
    }
    else
    {
        orm.g = roughness_factor;
        orm.b = metallic_factor;
    }

    // Normal
    if (has_normal_tex)
    {
        vec3 tmp_normal = texture(normal_tex, fs_in.uv).rgb;
        tmp_normal = normalize(normal * 2.0 - 1.0);
        nornal = normalize(fs_in.TBN * tmp_normal);
    }
    else
        normal = fs_in.normal;

    // Emissive
    if (has_emissive_tex)
        emissive = texture(emissive_tex, fs_in.uv).rgb;
    else
        emissive = emissive_factor;

    // Position
    position = fs_in.pos;
}