#pragma once

#include <memory>
#include <tiny_gltf.h>
#include <glm/glm.hpp>

class Texture;
class Program;

using shared_program = std::shared_ptr<Program>;

class Material
{
public:
    Material(const tinygltf::Material& material, const tinygltf::Model& model);
    ~Material();

    void bind(const shared_program program);
private:
    void parse_metallic_roughness(const tinygltf::PbrMetallicRoughness& pbr_params, const tinygltf::Model& model);
    void parse_specular_glossiness(const tinygltf::Value& pbr_params, const tinygltf::Model& model);

    bool m_is_metallic_roughness;

    // Base Color
    Texture* m_base_color = nullptr;
    glm::vec4 m_base_color_factor;
    int m_base_color_texcoord;
    
    // Metallic Roughness
    Texture* m_metallic_roughness = nullptr;
    float m_metallic_factor;
    float m_roughness_factor;
    int m_metallic_roughness_texcoord;
    
    // Normal
    Texture* m_normal = nullptr;
    int m_normal_texcoord;
    float m_normal_scale;
    
    // Emissive
    Texture* m_emissive = nullptr;
    glm::vec3 m_emissive_factor;
    int m_emissive_texcoord;

    // Occlusion
    Texture* m_occlusion = nullptr;
    float m_occlusion_factor = 1.f;
    float m_occlusion_strength;
    int m_occlusion_texcoord;

    // Specular Glossiness
    Texture* m_specular_glossiness = nullptr;
    glm::vec3 m_specular_factor = glm::vec3{1.f};
    float m_glossiness_factor = 1.f;
    int m_specular_glossiness_texcoord;
};