#include "material.hpp"

#include <glad/glad.h>
#include <iostream>

#include "program.hpp"
#include "texture.hpp"

Material::Material(const tinygltf::Material& material, const tinygltf::Model& model)
{
    // Set pbr val
    auto pbr_params = material.pbrMetallicRoughness;

    m_base_color_factor = glm::vec4{pbr_params.baseColorFactor[0], pbr_params.baseColorFactor[1], pbr_params.baseColorFactor[2], pbr_params.baseColorFactor[3]};
    m_emissive_factor   = glm::vec3{material.emissiveFactor[0], material.emissiveFactor[1], material.emissiveFactor[2]};
    m_metallic_factor   = static_cast<float>(pbr_params.metallicFactor);
    m_roughness_factor  = static_cast<float>(pbr_params.roughnessFactor);

    if (pbr_params.baseColorTexture.index != -1) {
        m_base_color          = new Texture(model.textures[pbr_params.baseColorTexture.index], pbr_params.baseColorTexture.texCoord, model);
        m_base_color_texcoord = pbr_params.baseColorTexture.texCoord;
        m_has_base_color_tex  = true;
    }
    if (pbr_params.metallicRoughnessTexture.index != -1) {
        m_metallic_roughness          = new Texture(model.textures[pbr_params.metallicRoughnessTexture.index], pbr_params.metallicRoughnessTexture.texCoord, model);
        m_metallic_roughness_texcoord = pbr_params.metallicRoughnessTexture.texCoord;
        m_has_metallic_roughness_tex  = true;
    }
    if (material.normalTexture.index != -1) {
        m_normal          = new Texture(model.textures[material.normalTexture.index], material.normalTexture.texCoord, model);
        m_normal_texcoord = material.normalTexture.texCoord;
        m_normal_scale    = material.normalTexture.scale;
        m_has_normal_tex  = true;
    }
    if (material.emissiveTexture.index != -1) {
        m_emissive          = new Texture(model.textures[material.emissiveTexture.index], material.emissiveTexture.texCoord, model);
        m_emissive_texcoord = material.emissiveTexture.texCoord;
        m_has_emissive_tex  = true;
    }
    if (material.occlusionTexture.index != -1) {
        m_occlusion          = new Texture(model.textures[material.occlusionTexture.index], material.occlusionTexture.texCoord, model);
        m_occlusion_texcoord = material.occlusionTexture.texCoord;
        m_occlusion_strength = material.occlusionTexture.strength;
        m_has_occlusion_tex  = true;
    }

}

Material::~Material()
{
    delete m_base_color;
    delete m_metallic_roughness;
    delete m_normal;
    delete m_emissive;
    delete m_occlusion;
}

void Material::bind(const shared_program program)
{
    program->use();

    if (!program->need_material_binding())
        return;
    // Bind factors;
    program->addUniformVec4(m_base_color_factor, "base_color_factor");
    program->addUniformVec3(m_emissive_factor, "emissive_factor");
    program->addUniformFloat(m_metallic_factor, "metallic_factor");
    program->addUniformFloat(m_roughness_factor, "roughness_factor");
    program->addUniformFloat(m_occlusion_factor, "occlusion_factor");

    // Bind textures information
    program->addUniformBool(m_has_base_color_tex, "has_base_color_texture");
    program->addUniformBool(m_has_metallic_roughness_tex, "has_metallic_roughness_texture");
    program->addUniformBool(m_has_normal_tex, "has_normal_texture");
    program->addUniformBool(m_has_emissive_tex, "has_emissive_texture");
    program->addUniformBool(m_has_occlusion_tex, "has_occlusion_texture");

    // Bind textures
    if (m_base_color)
    {
        m_base_color->bind(program, 0, "base_color_sampler");
        program->addUniformInt(m_base_color_texcoord, "base_color_texcoord");
    }
    if (m_metallic_roughness)
    {
        m_metallic_roughness->bind(program, 1, "metallic_roughness_sampler");
        program->addUniformInt(m_metallic_roughness_texcoord, "metallic_roughness_texcoord");
    }
    if (m_normal)
    {
        m_normal->bind(program, 2, "normal_sampler");
        program->addUniformInt(m_normal_texcoord, "normal_texcoord");
        program->addUniformFloat(m_normal_scale, "normal_scale");
    }
    if (m_emissive)
    {
        m_emissive->bind(program, 3, "emissive_sampler");
        program->addUniformInt(m_emissive_texcoord, "emissive_texcoord");
    }
    if (m_occlusion)
    {
        m_occlusion->bind(program, 4 ,"occlusion_sampler");
        program->addUniformInt(m_occlusion_texcoord, "occlusion_texcoord");
        program->addUniformFloat(m_occlusion_strength, "occlusion_strength");
    }
}