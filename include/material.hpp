#pragma once

#include <memory>
#include <tiny_gltf.h>
#include <glm/glm.hpp>

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

    unsigned int m_base_color;
    unsigned int m_metallic_roughness;
    unsigned int m_normal;
    // unsigned int m_emissive;
    // unsigned int m_ambient_occlusion;
};