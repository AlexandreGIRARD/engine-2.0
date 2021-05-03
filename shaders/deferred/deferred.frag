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

struct brdf_infos
{
    vec3  base_color;             // base color value at the surface fragment
    float roughness;              // roughness value at the surface fragment
    float metallic;               // metallic value at the surface fragment
    vec3  F0;                     // Indice Of Reflection
    float NdotL;                  // cos angle between normal and light direction
    float NdotV;                  // cos angle between normal and view direction
    float NdotH;                  // cos angle between normal and half vector
    float LdotH;                  // cos angle between light direction and half vector
    float VdotH;                  // cos angle between view direction and half vector
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
layout (binding = 6) uniform samplerCube ibl_irradiance;
layout (binding = 7) uniform samplerCube ibl_specular;

// Other uniforms
uniform vec3 cam_pos;

in vec2 frag_uv;

// Output Attachment
layout (location = 0) out vec4 deferred_output;

// Cook-Terance BRF equation
// More here http://graphicrants.blogspot.com/2013/08/specular-brdf-reference.html

// Normal Distribution Function
float NDF_GGX(brdf_infos infos)
{
    float square_alpha = infos.roughness * infos.roughness;
    float denum = (infos.NdotH * infos.NdotH) * (square_alpha - 1.0) + 1.0;
    return square_alpha / (PI * denum * denum);
}

// Geometry Function
float Geometry_Schlick_GGX(brdf_infos infos)
{
    float NdotV = infos.NdotV;
    float NdotL = infos.NdotL;
    float a     = infos.roughness;
    float ggx1 = 2.0 * NdotL / (NdotL + sqrt(a * a + (1.0 - a * a) * (NdotL * NdotL)));
    float ggx2 = 2.0 * NdotV / (NdotV + sqrt(a * a + (1.0 - a * a) * (NdotV * NdotV)));

    return ggx1 * ggx2;
}

// Fresnel Equation
vec3 Fresnel_Schlick(brdf_infos infos)
{
    return infos.F0 + (1.0 - infos.F0) * pow(clamp(1.0 - infos.VdotH, 0.0, 1.0), 5.0);
}

// Lambertian diffuse

vec3 Lambertian_Diffuse(brdf_infos infos)
{
    return infos.base_color / PI;
}

// ----------------------------------------------------------------------------
void main()
{		
    vec3 base_color = pow(texture(base_color_tex, frag_uv).rgb, vec3(2.2));
    float metallic  = texture(orm_tex, frag_uv).b;
    float roughness = texture(orm_tex, frag_uv).g;
    float ao        = texture(orm_tex, frag_uv).r;
    vec3 emissive   = texture(emissive_tex, frag_uv).rgb;
    vec3 world_pos  = texture(position_tex, frag_uv).xyz;
    vec3 V          = normalize(cam_pos - world_pos);
    vec3 N          = texture(normal_tex, frag_uv).xyz;


    // Init brdf infos
    brdf_infos infos;
    infos.roughness  = roughness;
    infos.metallic   = metallic;
    infos.F0         = mix(vec3(0.04), base_color, metallic);
    infos.base_color = base_color;

    // reflectance equation
    vec3 Lo = vec3(0.0);
    for(int i = 0; i < nb_point_lights; i++) 
    {
        // Compute BRDF infos
        vec3 L = normalize(point_lights[i].position - world_pos);
        vec3 H = normalize(V + L);

        brdf_infos infos;
        infos.NdotH = clamp(dot(N, H), 0.0, 1.0);
        infos.NdotV = clamp(dot(N, V), 0.0, 1.0);
        infos.NdotL = clamp(dot(N, V), 0.0, 1.0);
        infos.LdotH = clamp(dot(L, H), 0.0, 1.0);
        infos.VdotH = clamp(dot(V, H), 0.0, 1.0);

        // Radiance
        float distance = length(point_lights[i].position - world_pos);
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance = point_lights[i].color * attenuation;

        // Cook Torrance BRDF computation
        float D = NDF_GGX(infos);   
        float G = Geometry_Schlick_GGX(infos);      
        vec3  F = Fresnel_Schlick(infos);

        vec3 diffuse_contrib  = (1.0 - F) * Lambertian_Diffuse(infos);
        vec3 specular_contrib = (D * F * G) / (4.0 * infos.NdotV * infos.NdotL);

        // add to outgoing radiance Lo
        Lo += (diffuse_contrib + specular_contrib) * radiance * infos.NdotL;
    }   
    
    // // ambient lighting (we now use IBL as the ambient term)
    // vec3 kS = fresnelSchlick(max(dot(N, V), 0.0), F0);
    // vec3 kD = 1.0 - kS;
    // kD *= 1.0 - metallic;	  
    // vec3 irradiance = texture(ibl_irradiance, N).rgb;
    // vec3 diffuse      = irradiance * albedo;
    // vec3 ambient = (kD * diffuse) * ao;
    
    vec3 color = vec3(0.03) + Lo;

    color = mix(color, color * ao, 1.0);
	color += emissive;

    // HDR tonemapping
    color = color / (color + vec3(1.0));
    // gamma correct
    color = pow(color, vec3(1.0/2.2)); 

    deferred_output = vec4(color, 1.0);
    // deferred_output = vec4(texture(emissive_tex, frag_uv).rgb, 1);
}