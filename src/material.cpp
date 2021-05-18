#include "material.hpp"

#include <glad/glad.h>
#include <iostream>

#include "program.hpp"
#include "texture.hpp"

Material::Material(const tinygltf::Material& material, const tinygltf::Model& model)
{
    // Set pbr val
    auto it = material.extensions.find("KHR_materials_pbrSpecularGlossiness");
    if (it == material.extensions.end())
        parse_metallic_roughness(material.pbrMetallicRoughness, model);
    else
        parse_specular_glossiness(it->second, model);

    m_emissive_factor   = glm::vec3{material.emissiveFactor[0], material.emissiveFactor[1], material.emissiveFactor[2]};

    if (material.normalTexture.index != -1) {
        m_normal          = new Texture(model.textures[material.normalTexture.index], material.normalTexture.texCoord, model);
        m_normal_texcoord = material.normalTexture.texCoord;
        m_normal_scale    = material.normalTexture.scale;
    }
    if (material.emissiveTexture.index != -1) {
        m_emissive          = new Texture(model.textures[material.emissiveTexture.index], material.emissiveTexture.texCoord, model);
        m_emissive_texcoord = material.emissiveTexture.texCoord;
    }
    if (material.occlusionTexture.index != -1) {
        m_occlusion          = new Texture(model.textures[material.occlusionTexture.index], material.occlusionTexture.texCoord, model);
        m_occlusion_texcoord = material.occlusionTexture.texCoord;
        m_occlusion_strength = material.occlusionTexture.strength;
    }

}

void Material::parse_metallic_roughness(const tinygltf::PbrMetallicRoughness& pbr_params, const tinygltf::Model& model)
{
    std::cout << "Metallic-Roughness Model" << std::endl;
    m_is_metallic_roughness = true;

    m_base_color_factor = glm::vec4{pbr_params.baseColorFactor[0], pbr_params.baseColorFactor[1], pbr_params.baseColorFactor[2], pbr_params.baseColorFactor[3]};
    m_metallic_factor   = static_cast<float>(pbr_params.metallicFactor);
    m_roughness_factor  = static_cast<float>(pbr_params.roughnessFactor);

    if (pbr_params.baseColorTexture.index != -1) {
        m_base_color          = new Texture(model.textures[pbr_params.baseColorTexture.index], pbr_params.baseColorTexture.texCoord, model);
        m_base_color_texcoord = pbr_params.baseColorTexture.texCoord;
    }
    if (pbr_params.metallicRoughnessTexture.index != -1) {
        m_metallic_roughness          = new Texture(model.textures[pbr_params.metallicRoughnessTexture.index], pbr_params.metallicRoughnessTexture.texCoord, model);
        m_metallic_roughness_texcoord = pbr_params.metallicRoughnessTexture.texCoord;
    }
}

void Material::parse_specular_glossiness(const tinygltf::Value& pbr_params, const tinygltf::Model& model)
{
    std::cout << "Specular-Glossiness Model" << std::endl;
    m_is_metallic_roughness = false;

    // Diffuse as Albedo
    if (pbr_params.Has("diffuseTexture")) {
        int index = pbr_params.Get("diffuseTexture").Get("index").GetNumberAsInt();
        m_base_color          = new Texture(model.textures[index], 0, model);
        m_base_color_texcoord = 0;
    }
    if (pbr_params.Has("diffuseFactor")) {
        auto factor = pbr_params.Get("diffuseFactor");
        m_base_color_factor = glm::vec4{factor.Get(0).GetNumberAsDouble(), factor.Get(1).GetNumberAsDouble(), factor.Get(2).GetNumberAsDouble(), factor.Get(3).GetNumberAsDouble()};
    }

    // Specular Glossiness
    if (pbr_params.Has("specularGlossinessTexture")) {
        int index = pbr_params.Get("specularGlossinessTexture").Get("index").GetNumberAsInt();
        m_specular_glossiness          = new Texture(model.textures[index], 0, model);
        m_specular_glossiness_texcoord = 0;
    }
    if (pbr_params.Has("specularFactor")) {
        auto factor = pbr_params.Get("specular");
        m_specular_factor = glm::vec3{factor.Get(0).GetNumberAsDouble(), factor.Get(1).GetNumberAsDouble(), factor.Get(2).GetNumberAsDouble()};
    }
    if (pbr_params.Has("glossinessFactor")) {
        m_glossiness_factor = pbr_params.Get("glossinessFactor").GetNumberAsDouble();
    }
}


Material::~Material()
{
    delete m_base_color;
    delete m_metallic_roughness;
    delete m_normal;
    delete m_emissive;
    delete m_occlusion;
    delete m_specular_glossiness;
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
    program->addUniformVec3(m_specular_factor, "specular_factor");
    program->addUniformFloat(m_glossiness_factor, "glossiness_factor");

    // Bind textures information
    program->addUniformBool(m_is_metallic_roughness, "is_metallic_roughness");
    program->addUniformBool(m_base_color != nullptr, "has_base_color_texture");
    program->addUniformBool(m_metallic_roughness != nullptr, "has_metallic_roughness_texture");
    program->addUniformBool(m_normal != nullptr, "has_normal_texture");
    program->addUniformBool(m_emissive != nullptr, "has_emissive_texture");
    program->addUniformBool(m_occlusion != nullptr, "has_occlusion_texture");
    program->addUniformBool(m_specular_glossiness != nullptr, "has_specular_glossiness_texture");

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
    if (m_specular_glossiness)
    {
        m_specular_glossiness->bind(program, 5, "specular_glossiness_sampler");
        program->addUniformInt(m_specular_glossiness_texcoord, "specular_glossiness_texcoord");
    }
}