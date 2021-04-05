#pragma once

#include <memory>
#include <tiny_gltf.h>
#include <glm/glm.hpp>

class Texture;
class Program;

class Material
{
public:
    Material(const tinygltf::Material& material, const tinygltf::Model& model);
    ~Material();

    void bind(const Program& program);
private:
    glm::vec3 m_base_color_factor;
    double m_metallic_factor;
    double m_roughness_factor;

    Texture* m_base_color = nullptr;
    Texture* m_metallic_roughness = nullptr;
    Texture* m_normal = nullptr;
    // Texture* m_emissive;
    // Texture* m_ambient_occlusion;
};