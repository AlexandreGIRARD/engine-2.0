#pragma once

#include <tiny_gltf.h>

class Program;

class Texture
{
public:
    Texture(const tinygltf::Texture& tex, const int tex_coord, const tinygltf::Model& model);
    ~Texture();
    void bind(const Program& program, unsigned int location, const char* name);
private:
    unsigned int m_id;
    unsigned int m_tex_coord;
};