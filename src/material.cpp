#include "material.hpp"

#include <glad/glad.h>
#include <iostream>

#include "program.hpp"
#include "texture.hpp"

static int cpt = 0;

Material::Material(const tinygltf::Material& material, const tinygltf::Model& model)
{
    std::cout << cpt++ << std::endl;

    // Set pbr val
    auto pbr_params = material.pbrMetallicRoughness;

    m_base_color_factor = glm::vec3{pbr_params.baseColorFactor[0], pbr_params.baseColorFactor[1], pbr_params.baseColorFactor[2]};
    m_metallic_factor = pbr_params.metallicFactor;
    m_roughness_factor = pbr_params.roughnessFactor;

    if (pbr_params.baseColorTexture.index != -1)
        m_base_color         = new Texture(model.textures[pbr_params.baseColorTexture.index], pbr_params.baseColorTexture.texCoord, model);
    if (pbr_params.metallicRoughnessTexture.index != -1)
        m_metallic_roughness = new Texture(model.textures[pbr_params.metallicRoughnessTexture.index], pbr_params.metallicRoughnessTexture.texCoord, model);
    if (material.normalTexture.index != -1)
        m_normal             = new Texture(model.textures[material.normalTexture.index], material.normalTexture.texCoord, model);
    // m_emissive           = create_texture(material.emissiveTexture.index, model);
    // m_ambient_occlusion  = create_texture(material.occlusionTexture.index, model);
     
}

Material::~Material()
{
    delete m_base_color;
    delete m_metallic_roughness;
    delete m_normal;
}

void Material::bind(const Program& program)
{
    if (!program.need_material_binding())
        return;

    if (m_base_color)
        m_base_color->bind(program, 0, "base_color");
    if (m_metallic_roughness)
        m_metallic_roughness->bind(program, 1, "metallic_roughness");
    if (m_normal)
        m_normal->bind(program, 2, "normal");
}