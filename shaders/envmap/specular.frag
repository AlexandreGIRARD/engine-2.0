#version 450 core

const float PI = 3.14159265359f;

layout (binding = 0) uniform samplerCube skybox;
uniform float roughness;

in vec3 pos;

out vec4 color;

// Normal Distribution Function
float NDF_GGX(float NdotH, float roughness)
{
    float square_alpha = roughness * roughness;
    float denum = (NdotH * NdotH) * (square_alpha - 1.0) + 1.0;
    return square_alpha / (PI * denum * denum);
}

// http://holger.dammertz.org/stuff/notes_HammersleyOnHemisphere.html
// efficient VanDerCorpus calculation.
float radical_inverse_VdC(uint bits) 
{
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}
// ----------------------------------------------------------------------------
vec2 hammersley(uint i, uint N)
{
    return vec2(float(i)/float(N), radical_inverse_VdC(i));
}

vec3 importance_sample_GGX(vec2 Xi, vec3 N, float roughness)
{
    float a = roughness*roughness;
	
    float phi = 2.0 * PI * Xi.x;
    float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a*a - 1.0) * Xi.y));
    float sinTheta = sqrt(1.0 - cosTheta*cosTheta);
	
    // from spherical coordinates to cartesian coordinates
    vec3 H;
    H.x = cos(phi) * sinTheta;
    H.y = sin(phi) * sinTheta;
    H.z = cosTheta;
	
    // from tangent-space vector to world-space sample vector
    vec3 U = abs(N.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
    vec3 T = normalize(cross(U, N));
    vec3 B = cross(N, T);
	
    vec3 sampleVec = T * H.x + B * H.y + N * H.z;
    return normalize(sampleVec);
}  

void main()
{
    vec3 N = normalize(pos);
    
    // make the simplyfying assumption that V equals R equals the normal 
    vec3 R = N;
    vec3 V = R;

    const uint SAMPLE_COUNT = 1024u;
    vec3 prefilteredColor = vec3(0.0);
    float totalWeight = 0.0;
    
    for(uint i = 0u; i < SAMPLE_COUNT; ++i)
    {
        // generates a sample vector that's biased towards the preferred alignment direction (importance sampling).
        vec2 Xi = hammersley(i, SAMPLE_COUNT);
        vec3 H = importance_sample_GGX(Xi, N, roughness);
        vec3 L  = normalize(2.0 * dot(V, H) * H - V);

        float NdotL = max(dot(N, L), 0.0);
        float NdotH = max(dot(N, H), 0.0);
        if(NdotL > 0.0)
        {
            // sample from the environment's mip level based on roughness/pdf
            float D    = NDF_GGX(NdotH, roughness);
            float HdotV = max(dot(H, V), 0.0);
            float pdf = D * NdotH / (4.0 * HdotV) + 0.0001; 

            float resolution = 512.0; // resolution of source cubemap (per face)
            float saTexel  = 4.0 * PI / (6.0 * resolution * resolution);
            float saSample = 1.0 / (float(SAMPLE_COUNT) * pdf + 0.0001);

            float mipLevel = roughness == 0.0 ? 0.0 : 0.5 * log2(saSample / saTexel); 
            
            prefilteredColor += textureLod(skybox, L, mipLevel).rgb * NdotL;
            totalWeight      += NdotL;
        }
    }

    prefilteredColor = prefilteredColor / totalWeight;

    color = vec4(prefilteredColor, 1.0);
}