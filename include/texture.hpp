#pragma once

#include <tiny_gltf.h>

#include <memory>

class Program;

using shared_program = std::shared_ptr<Program>;

class Texture
{
public:
    Texture(const tinygltf::Texture& tex, const int tex_coord, const tinygltf::Model& model);
    ~Texture();
    void bind(const shared_program program, unsigned int location, const char* name);
private:
    unsigned int m_id;
    unsigned int m_tex_coord;
};