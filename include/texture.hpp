#pragma once

#include <tiny_gltf.h>

#include <memory>

class Program;

using shared_program = std::shared_ptr<Program>;

struct Texture_Info
{
    unsigned int width;
    unsigned int height;
    unsigned int wrap_s;
    unsigned int wrap_t;
    unsigned int min_filter;
    unsigned int mag_filter;
    unsigned int internal_format;
    unsigned int format;
    unsigned int type;
};

class Texture
{
public:
    Texture(Texture_Info& infos, char* data);
    Texture(const std::string& file_path, bool as_float);
    Texture(const tinygltf::Texture& tex, const int tex_coord, const tinygltf::Model& model);
    ~Texture();
    void bind(const shared_program program, unsigned int location, const char* name);
private:
    unsigned int m_id;
    unsigned int m_tex_coord;
};