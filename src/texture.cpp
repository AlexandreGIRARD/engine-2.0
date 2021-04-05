#include "texture.hpp"

#include <glad/glad.h>

#include "program.hpp"

Texture::Texture(const tinygltf::Texture& tex, int tex_coord, const tinygltf::Model& model)
    : m_tex_coord(tex_coord)
{
    glGenTextures(1, &m_id);
    glBindTexture(GL_TEXTURE_2D, m_id);

    // Texture parameter -> Bilinear and repeat
    auto sampler = model.samplers[tex.sampler];
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, sampler.wrapS);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, sampler.wrapT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, sampler.minFilter != -1 ? sampler.minFilter : GL_NEAREST_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, sampler.magFilter != -1 ? sampler.magFilter : GL_LINEAR);

    int width, height, nb_channels;
    tinygltf::Image image = model.images[model.textures[tex.source].source];

    width  = image.width;
    height = image.height;
    nb_channels = image.component;

    unsigned char* data = image.image.data();
    
    if (nb_channels == 3)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    else
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
}

Texture::~Texture()
{
    glDeleteTextures(1, &m_id);
}

void Texture::bind(const Program& program, unsigned int location, const char* name)
{
    program.addUniformTexture(location, name);
    glActiveTexture(GL_TEXTURE0 + location);
    glBindTexture(GL_TEXTURE_2D, m_id);
}