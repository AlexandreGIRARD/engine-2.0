#include "material.hpp"

#include <glad/glad.h>

static uint create_texture(int index, const tinygltf::Model& model)
{
    if (index == -1)
        return -1;
    uint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);

    // Texture parameter -> Bilinear and repeat
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nb_channels;
    tinygltf::Image image = model.images[model.textures[index].source];

    width  = image.width;
    height = image.height;
    nb_channels = image.component;

    unsigned char* data = image.image.data();

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    return tex;
}

Material::Material(const tinygltf::Material& material, const tinygltf::Model& model)
{
    int br = 0;

    // Set pbr val
    auto pbr_params = material.pbrMetallicRoughness;

    m_base_color_factor = glm::vec3{pbr_params.baseColorFactor[0], pbr_params.baseColorFactor[1], pbr_params.baseColorFactor[2]};
    m_metallic_factor = pbr_params.metallicFactor;
    m_roughness_factor = pbr_params.roughnessFactor;

    m_base_color         = create_texture(pbr_params.baseColorTexture.index, model);
    m_metallic_roughness = create_texture(pbr_params.metallicRoughnessTexture.index, model);
    m_normal             = create_texture(material.normalTexture.index, model);
    // m_emissive           = create_texture(material.emissiveTexture.index, model);
    // m_ambient_occlusion  = create_texture(material.occlusionTexture.index, model);
     
}

Material::~Material()
{
    uint textures[] = {m_base_color, m_metallic_roughness, m_normal};
    glDeleteTextures(3, textures);
}