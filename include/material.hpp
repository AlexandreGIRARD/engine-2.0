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
    // Base Color
    Texture* m_base_color = nullptr;
    bool m_has_base_color_tex = false;
    glm::vec4 m_base_color_factor;
    int m_base_color_texcoord;
    
    // Metallic Roughness
    Texture* m_metallic_roughness = nullptr;
    bool m_has_metallic_roughness_tex = false;
    float m_metallic_factor;
    float m_roughness_factor;
    int m_metallic_roughness_texcoord;
    
    // Normal
    Texture* m_normal = nullptr;
    bool m_has_normal_tex = false;
    int m_normal_texcoord;
    float m_normal_scale;
    
    // Emissive
    Texture* m_emissive = nullptr;
    bool m_has_emissive_tex = false;
    glm::vec3 m_emissive_factor;
    int m_emissive_texcoord;

    // Occlusion
    Texture* m_occlusion = nullptr;
    bool m_has_occlusion_tex = false;
    float m_occlusion_factor = 1.f;
    float m_occlusion_strength;
    int m_occlusion_texcoord;
};