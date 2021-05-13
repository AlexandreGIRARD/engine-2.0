#pragma once

#include <memory>

#include <glm/glm.hpp>
#include <tiny_gltf.h>

class Program;

using shared_program = std::shared_ptr<Program>;

enum Light_Type
{
    DIRECTIONAL_LIGHT,
    POINT_LIGHT,
    SPOT_LIGHT,
};

class Light
{
public:
    Light(const tinygltf::Light& light);
    ~Light();

    const Light_Type get_type() { return m_type; }

    void bind(shared_program& program, const int id);

    void set_direction(const glm::vec3& direction) { m_direction = direction; }
    void set_position(const glm::vec3& position) { m_position = position; }
private:
    Light_Type m_type;
    glm::vec3 m_color;
    float m_intensity;
    float m_range;

    // Spot exclusive
    float m_inner_angle;
    float m_outer_angle;

    // Spot/Directional
    glm::vec3 m_direction;

    // Point
    glm::vec3 m_position;
};