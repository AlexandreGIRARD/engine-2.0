#version 450 core

const vec2 invAtan = vec2(0.1591, 0.3183);

layout (binding = 0) uniform samplerCube skybox;

in vec3 pos;

out vec4 frag_color;

void main()
{
    vec3 N = normalize(pos);

    vec3 irradiance = vec3(0.0);   
    
    // tangent space calculation from origin point
    vec3 up    = vec3(0.0, 1.0, 0.0);
    vec3 right = normalize(cross(up, N));
    up         = normalize(cross(N, right));
       
    float sample_delta = 0.025;
    float nb_samples = 0.0;
    for(float phi = 0.0; phi < 2.0 * PI; phi += sample_delta)
    {
        for(float theta = 0.0; theta < 0.5 * PI; theta += sample_delta)
        {
            // spherical to cartesian (in tangent space)
            vec3 tangent_sample = vec3(sin(theta) * cos(phi),  sin(theta) * sin(phi), cos(theta));
            // tangent space to world
            vec3 sample_vec = tangent_sample.x * right + tangent_sample.y * up + tangent_sample.z * N; 

            irradiance += texture(skybox, sample_vec).rgb * cos(theta) * sin(theta);
            nb_samples++;
        }
    }
    irradiance = PI * irradiance * (1.0 / float(nb_samples));
    
    frag_color = vec4(irradiance, 1.0);
}