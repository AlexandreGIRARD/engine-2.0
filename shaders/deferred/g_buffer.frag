#version 450 core

// Base Color
layout (binding = 0) uniform sampler2D base_color_sampler;
uniform int base_color_texcoord;
uniform vec4  base_color_factor;
uniform int has_base_color_texture;

// Metallic Roughness
layout (binding = 1) uniform sampler2D metallic_roughness_sampler;
uniform int metallic_roughness_texcoord;
uniform float metallic_factor;
uniform float roughness_factor;
uniform int has_metallic_roughness_texture;

// Normal
layout (binding = 2) uniform sampler2D normal_sampler;
uniform int normal_texcoord;
uniform float normal_scale;
uniform int has_normal_texture;

// Emissive
layout (binding = 3) uniform sampler2D emissive_sampler;
uniform int emissive_texcoord;
uniform vec3  emissive_factor;
uniform int has_emissive_texture;

// Occlusion
layout (binding = 4) uniform sampler2D occlusion_sampler;
uniform int occlusion_texcoord;
uniform float occlusion_factor;
uniform float occlusion_strength;
uniform int has_occlusion_texture;

// Vertex Shader Outputs
in VS_OUT
{
    vec4 pos;
    vec3 normal;
    vec2 uv;
    mat3 TBN;
    vec2[6] tex_coords;
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
    if (has_base_color_texture != 0)
        base_color = texture(base_color_sampler, fs_in.tex_coords[base_color_texcoord]).rgba;
    else
        base_color = base_color_factor;
    
    // Temporary for non-opaque 
    if (base_color.a == 0)
        discard;

    // Occlusion
    if (has_occlusion_texture != 0)
        orm.r = texture(occlusion_sampler, fs_in.tex_coords[occlusion_texcoord]).r;
    else
        orm.r = occlusion_factor;
    
    // Metallic Roughness
    if (has_metallic_roughness_texture != 0)
    {
        orm.g = texture(metallic_roughness_sampler, fs_in.tex_coords[metallic_roughness_texcoord]).g;
        orm.b = texture(metallic_roughness_sampler, fs_in.tex_coords[metallic_roughness_texcoord]).b;
    }
    else
    {
        orm.g = roughness_factor;
        orm.b = metallic_factor;
    }

    // Normal
    if (has_normal_texture != 0)
    {
        vec3 tmp_normal = texture(normal_sampler, fs_in.tex_coords[normal_texcoord]).rgb;
        tmp_normal = normalize((tmp_normal * 2.0 - 1.0) * vec3(normal_scale, normal_scale, 1.0));
        normal = normalize(fs_in.TBN * tmp_normal);
    }
    else
    {
        normal = fs_in.normal;
    }


    // Emissive
    if (has_emissive_texture != 0)
        emissive = texture(emissive_sampler, fs_in.tex_coords[emissive_texcoord]).rgb;
    else
        emissive = emissive_factor;

    // Position
    position = fs_in.pos;
}