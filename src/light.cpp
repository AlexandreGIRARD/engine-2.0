#include "light.hpp"

#include "program.hpp"

Light::Light(const tinygltf::Light& light)
{
    // Determine Light type
    if (light.type == "directional")
        m_type = Light_Type::DIRECTIONAL;
    else if (light.type == "point")
        m_type = Light_Type::POINT;
    else
        m_type = Light_Type::SPOT;

    m_color = glm::vec3{light.color[0], light.color[1], light.color[2]};
    m_intensity = static_cast<float>(light.intensity);
    m_range = static_cast<float>(light.range);

    if (m_type == Light_Type::SPOT) 
    {
        m_inner_angle = static_cast<float>(light.spot.innerConeAngle);
        m_outer_angle = static_cast<float>(light.spot.outerConeAngle);
    }
}

Light::~Light()
{}

void Light::bind(shared_program& program, const int id)
{
    std::string name;
    if (m_type == Light_Type::POINT)
    {
        name = "point_lights[" + std::to_string(id) + "]";
        program->addUniformVec3(m_position, (name + ".position").c_str());
    }
    else if (m_type == Light_Type::SPOT)
    {
        name = "spot_lights[" + std::to_string(id) + "]";
        program->addUniformVec3(m_direction, (name + ".direction").c_str());
        program->addUniformFloat(m_inner_angle, (name + ".inner_angle").c_str());
        program->addUniformFloat(m_outer_angle, (name + ".outer_angle").c_str());
    }
    else
    {
        name = "sun";
        program->addUniformVec3(m_direction, "sun.direction");
    }
    program->addUniformVec3(m_color, (name + ".color").c_str());
    program->addUniformFloat(m_intensity, (name + ".intensity").c_str());
    program->addUniformFloat(m_range, (name + ".range").c_str());
}