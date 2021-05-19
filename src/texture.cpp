#include "texture.hpp"

#include <glad/glad.h>
#include <stb_image.h>

#include "program.hpp"

Texture::Texture(Texture_Info& infos, char* data)
{
    glGenTextures(1, &m_id);
    glBindTexture(GL_TEXTURE_2D, m_id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, infos.wrap_s);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, infos.wrap_t);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, infos.min_filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, infos.mag_filter);

    glTexImage2D(GL_TEXTURE_2D, 0, infos.internal_format, infos.width, infos.height, 0, infos.format, infos.type, data);
}

Texture::Texture(const std::string& file_path, bool as_float)
{
    glGenTextures(1, &m_id);
    glBindTexture(GL_TEXTURE_2D, m_id);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nb_channels;
    void* data;
    GLenum internal_format, format, type;

    stbi_set_flip_vertically_on_load(true);

    if (as_float)
    {
        data = stbi_loadf(file_path.c_str(), &width, &height, &nb_channels, 0);
        if (nb_channels == 3)
        {
            internal_format = GL_RGB16F;
            format = GL_RGB;
            type = GL_FLOAT;
        }
        else
        {
            internal_format = GL_RGBA16F;
            format = GL_RGBA;
            type = GL_FLOAT;
        }
    }
    else
    {
        data = stbi_load(file_path.c_str(), &width, &height, &nb_channels, 0);
        if (nb_channels == 3)
        {
            internal_format = GL_RGB;
            format = GL_RGB;
            type = GL_UNSIGNED_BYTE;
        }
        else
        {
            internal_format = GL_RGBA;
            format = GL_RGBA;
            type = GL_UNSIGNED_BYTE;
        }
    }

    glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width, height, 0, format, type, data);

    stbi_image_free(data);
}

Texture::Texture(const tinygltf::Texture& tex, int tex_coord, const tinygltf::Model& model)
    : m_tex_coord(tex_coord)
{
    glGenTextures(1, &m_id);
    glBindTexture(GL_TEXTURE_2D, m_id);

    // Texture parameter -> Bilinear and repeat
    if (tex.sampler != -1)
    {
        auto sampler = model.samplers[tex.sampler];
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, sampler.wrapS);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, sampler.wrapT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, sampler.minFilter != -1 ? sampler.minFilter : GL_NEAREST_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, sampler.magFilter != -1 ? sampler.magFilter : GL_LINEAR);
    }
    else
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    tinygltf::Image image = model.images[model.textures[tex.source].source];
    

    int width  = image.width;
    int height = image.height;
    int nb_channels = image.component;

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

void Texture::bind(const shared_program program, unsigned int location, const char* name)
{
    program->addUniformTexture(location, name);
    glActiveTexture(GL_TEXTURE0 + location);
    glBindTexture(GL_TEXTURE_2D, m_id);
}