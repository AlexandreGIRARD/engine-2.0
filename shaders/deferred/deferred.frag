#version 450 core

#define MAX_POINT_LIGHT 12
#define MAX_SPOT_LIGHT 1

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

// Other uniforms
const vec3 cam_pos = vec3(0.f, 0.f, 0.f);

in vec2 frag_uv;

// Output Attachment
layout (location = 0) out vec4 deferred_output;

const float PI = 3.14159265359;

// ----------------------------------------------------------------------------
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(max(1.0 - cosTheta, 0.0), 5.0);
}
// ----------------------------------------------------------------------------
void main()
{		
    vec3 albedo     = pow(texture(base_color_tex, frag_uv).rgb, vec3(2.2));
    float metallic  = texture(orm_tex, frag_uv).b;
    float roughness = texture(orm_tex, frag_uv).g;
    float ao        = texture(orm_tex, frag_uv).r;
    vec3 N          = texture(normal_tex, frag_uv).xyz;
    vec3 world_pos  = texture(position_tex, frag_uv).xyz;
    vec3 V = normalize(cam_pos - world_pos);

    // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0 
    // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)    
    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);

    // reflectance equation
    vec3 Lo = vec3(0.0);
    // for(int i = 0; i < nb_point_lights; i++) 
    // {
    //     // calculate per-light radiance
    //     vec3 L = normalize(point_lights[i].position - world_pos);
    //     vec3 H = normalize(V + L);
    //     float distance = length(point_lights[i].position - world_pos);
    //     float attenuation = 1.0 / (distance * distance);
    //     vec3 radiance = point_lights[i].color * attenuation;

    //     // Cook-Torrance BRDF
    //     float NDF = DistributionGGX(N, H, roughness);   
    //     float G   = GeometrySmith(N, V, L, roughness);      
    //     vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);
           
    //     vec3 nominator    = NDF * G * F; 
    //     float denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001; // 0.001 to prevent divide by zero.
    //     vec3 specular = nominator / denominator;
        
    //     // kS is equal to Fresnel
    //     vec3 kS = F;
    //     // for energy conservation, the diffuse and specular light can't
    //     // be above 1.0 (unless the surface emits light); to preserve this
    //     // relationship the diffuse component (kD) should equal 1.0 - kS.
    //     vec3 kD = vec3(1.0) - kS;
    //     // multiply kD by the inverse metalness such that only non-metals 
    //     // have diffuse lighting, or a linear blend if partly metal (pure metals
    //     // have no diffuse light).
    //     kD *= 1.0 - metallic;	  

    //     // scale light by NdotL
    //     float NdotL = max(dot(N, L), 0.0);        

    //     // add to outgoing radiance Lo
    //     Lo += (kD * albedo / PI + specular) * radiance * NdotL;  // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
    // }   
    
    // ambient lighting (note that the next IBL tutorial will replace 
    // this ambient lighting with environment lighting).
    vec3 ambient = vec3(0.03) * albedo * ao;
    
    vec3 color = ambient + Lo;

    // HDR tonemapping
    color = color / (color + vec3(1.0));
    // gamma correct
    color = pow(color, vec3(1.0/2.2)); 

    deferred_output = vec4(N, 1.0);
}