#version 450 core

layout (binding = 0) uniform sampler2D ao_tex;

uniform int size = 4; // use size of noise texture

in vec2 frag_uv; // input from vertex shader

out float blured;

void main() 
{
   vec2 texel_size = 1.0 / vec2(textureSize(ao_tex, 0));
   float result = 0.0;
   vec2 hlim = vec2(float(size) * 0.5 + 0.5);
   for (int i = 0; i < size; i++) 
   {
      for (int j = 0; j < size; j++) 
      {
         vec2 offset = (hlim + vec2(float(i), float(j))) * texel_size;
         result += texture(ao_tex, frag_uv + offset).r;
      }
   }
 
   blured = result / float(size * size);
}