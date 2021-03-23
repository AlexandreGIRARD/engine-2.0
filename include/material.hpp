#pragma once

#include <memory>
#include <tiny_gltf.h>
#include <glm/glm.hpp>

class Material
{
public:
    Material(const tinygltf::Material& material, const tinygltf::Model& model);
    ~Material();
private:
    glm::vec3 m_base_color_factor;
    double m_metallic_factor;
    double m_roughness_factor;

    uint m_base_color;
    uint m_metallic_roughness;
    uint m_normal;
    // uint m_emissive;
    // uint m_ambient_occlusion;
};