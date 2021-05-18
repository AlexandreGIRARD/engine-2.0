#version 450 core

#define MAX_POINT_LIGHT 12
#define MAX_SPOT_LIGHT 1

const float PI = 3.14159265359;

struct fragment_info
{
    vec4 position;
    vec4 base_color;
    vec3 emissive;
    vec3 normal;
    float roughness;
    float metallic;
    float occlusion;
};

struct point_light
{
    vec3 color;
    vec3 position;
    float intensity;
    float range;
};

struct spot_light
{
    vec3 color;
    vec3 direction;
    float intensity;
    float range;
    float inner_angle;
    float outer_angle;
};

struct sun_light
{
    vec3 color;
    vec3 direction;
    float intensity;
    float range;
};

// G-Buffer textures
layout (binding = 0) uniform sampler2D position_tex;
layout (binding = 1) uniform sampler2D base_color_tex;
layout (binding = 2) uniform sampler2D orm_tex;
layout (binding = 3) uniform sampler2D emissive_tex;
layout (binding = 4) uniform sampler2D normal_tex;

// SSAO texture
layout (binding = 5) uniform sampler2D ssao_tex;

// Shadow Map texture

// Lights
uniform sun_light sun;
uniform point_light[MAX_POINT_LIGHT] point_lights;
uniform spot_light[MAX_SPOT_LIGHT] spot_lights; 
uniform int nb_point_lights;
uniform int nb_spot_lights;

// IBL
layout (binding = 6) uniform samplerCube ibl_diffuse;
layout (binding = 7) uniform samplerCube ibl_specular;
layout (binding = 8) uniform sampler2D   brdf_lut;
uniform float ibl_factor;

// Other uniforms
uniform vec3 cam_pos;

in vec2 frag_uv;

// Output Attachment
layout (location = 0) out vec3 deferred_output;

// Cook-Terance BRF equation
// More here http://graphicrants.blogspot.com/2013/08/specular-brdf-reference.html

// Normal Distribution Function
float NDF_GGX(float NdotH, float roughness)
{
    float square_alpha = roughness * roughness;
    float denum = (NdotH * NdotH) * (square_alpha - 1.0) + 1.0;
    return square_alpha / (PI * denum * denum);
}

// Geometry Function
float Geometry_Smith_GGX(float NdotV, float NdotL, float roughness)
{
    float k = (roughness + 1.f) * (roughness + 1.f) / 8.f;
    float ggx1 = NdotL / (NdotL * (1 - k) + k);
    float ggx2 = NdotV / (NdotV * (1 - k) + k);

    return ggx1 * ggx2;
}

// Fresnel Equation
vec3 Fresnel_Schlick(vec3 F0, float VdotH)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - VdotH, 0.0, 1.0), 5.0);
}

vec3 Fresnel_Schlick_Roughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(max(1.0 - cosTheta, 0.0), 5.0);
}  

// Lambertian diffuse

vec3 Lambertian_Diffuse(vec3 base_color)
{
    return base_color / PI;
}

vec3 get_ibl_contribution(float roughness, vec3 base_color, vec3 F, float NdotV, vec3 N, vec3 R)
{
    float mip_count = 4.0; // resolution of 512x512
    float lod = (roughness * mip_count);
	
    vec2 brdf = texture(brdf_lut, vec2(NdotV, roughness)).rg;

    vec3 diffuse_ibl  = texture(ibl_diffuse, N).rgb;
    vec3 specular_ibl = textureLod(ibl_specular, R, lod).rgb;

    vec3 diffuse  = diffuse_ibl  * base_color;
    vec3 specular = specular_ibl * (F * brdf.x + brdf.y);

    return (1 - F) * diffuse + specular;
}

void main()
{		
    vec3 base_color = pow(texture(base_color_tex, frag_uv).rgb, vec3(2.2));
    float ao        = texture(ssao_tex, frag_uv).r;
    vec3 emissive   = pow(texture(emissive_tex, frag_uv).rgb, vec3(2.2));
    vec3 world_pos  = texture(position_tex, frag_uv).xyz;
    vec3 V          = normalize(cam_pos - world_pos);
    vec3 N          = texture(normal_tex, frag_uv).xyz;
    float NdotV     = clamp(dot(N, V), 0.0, 1.0);

    vec3 C;
    vec3 F0;
    float roughness;

    // Exectue as MetallicRoughness or SpecularGlossiness Workflow
    int is_metallica_roughness = int(texture(emissive_tex, frag_uv).a);
    if (is_metallica_roughness == 1)
    {
        C = base_color;
        float metallic  = texture(orm_tex, frag_uv).b;
        F0 = mix(vec3(0.04), base_color, metallic);
        roughness = texture(orm_tex, frag_uv).g;
    }
    else
    {
        vec4 spec_gloss = texture(orm_tex, frag_uv);
        C = base_color * (1 - max(spec_gloss.r, max(spec_gloss.g, spec_gloss.b)));
        F0 = spec_gloss.rgb;
        roughness = 1 - spec_gloss.a;
    }



    // reflectance equation
    vec3 color = vec3(0.0);
    for(int i = 0; i < nb_point_lights; i++) 
    {
        // Compute BRDF infos
        vec3 L = normalize(point_lights[i].position - world_pos);
        vec3 H = normalize(V + L);

        float NdotH = clamp(dot(N, H), 0.0, 1.0);
        float NdotL = clamp(dot(N, L), 0.0, 1.0);
        float VdotH = clamp(dot(V, H), 0.0, 1.0);

        // Radiance
        float distance = length(point_lights[i].position - world_pos);
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance = point_lights[i].color * attenuation;

        // Cook Torrance BRDF computation
        float D = NDF_GGX(NdotH, roughness);   
        float G = Geometry_Smith_GGX(NdotV, NdotL, roughness);      
        vec3  F = Fresnel_Schlick(F0, VdotH);

        vec3 diffuse_contrib  = (vec3(1.0) - F) * Lambertian_Diffuse(C);
        vec3 specular_contrib = (D * F * G) / (4.0 * NdotV * NdotL + 0.001);

        // add to outgoing radiance Lo
        color += (diffuse_contrib + specular_contrib) * radiance * NdotL;
    }   
    
    vec3 R = normalize(-reflect(V, N)); // Relection vector
    vec3 F = Fresnel_Schlick_Roughness(NdotV, F0, roughness);
    color += get_ibl_contribution(roughness, C, F, NdotV, N, R) * ibl_factor;

    color = mix(color, color * ao, 1.0);
	color += emissive;

    deferred_output = color;
}