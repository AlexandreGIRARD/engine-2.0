#include "pipeline/deferred_pass.hpp"

#include <glad/glad.h>

#include "light.hpp"

using namespace pipeline;

Deferred_Pass::Deferred_Pass(unsigned int width, unsigned int height)
    : Screen_Pass(width, height)    
{
    // Init program
    m_program->add_shader("utils/screen_quad.vert", GL_VERTEX_SHADER);
    m_program->add_shader("deferred/deferred.frag", GL_FRAGMENT_SHADER);
    m_program->link();

    // Init attachments
    m_attach_output = std::make_shared<Attachment>(GL_TEXTURE_2D, m_width, m_height, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE);

    // Init framebuffer
    m_fbo->bind();
    m_fbo->set_attachment(m_attach_output, GL_COLOR_ATTACHMENT0);
}

Deferred_Pass::~Deferred_Pass()
{
}

const std::vector<shared_attachment> Deferred_Pass::get_attachments()
{
    return std::vector<shared_attachment>{m_attach_output};
}

void Deferred_Pass::resize(unsigned int width, unsigned int height)
{
    Render_Pass::resize(width, height);
    m_attach_output->resize(width, height);
}

void Deferred_Pass::render(Camera* camera, Scene* scene)
{
    m_program->use();
    glm::vec3 cam_pos = camera->get_position();
    m_program->addUniformVec3(cam_pos, "cam_pos");
    m_program->addUniformFloat(m_ibl_factor, "ibl_factor");

    render_screen_quad();
}

void Deferred_Pass::set_lights(std::vector<shared_light> lights)
{
    m_program->use();

    int id_point_light = 0;
    int id_spot_light  = 0;
    for (auto light : lights)
    {
        switch (light->get_type())
        {
        case Light_Type::POINT_LIGHT:
            light->bind(m_program, id_point_light++);
            break;
        case Light_Type::SPOT_LIGHT:
            light->bind(m_program, id_spot_light++);
        default:
            light->bind(m_program, 0);
            break;
        }
    }

    m_program->addUniformInt(id_point_light, "nb_point_lights");
    m_program->addUniformInt(id_spot_light, "nb_spot_light");
}

void Deferred_Pass::set_gbuffer_attachments(const std::vector<shared_attachment> g_buffer_attachments)
{
    m_program->use();

    // Position texture
    m_program->addUniformTexture(0, "position_tex");
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, g_buffer_attachments[0]->m_name);

    // Base Color texture
    m_program->addUniformTexture(1, "base_color_tex");
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, g_buffer_attachments[1]->m_name);

    // Position texture
    m_program->addUniformTexture(2, "orm_tex");
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, g_buffer_attachments[2]->m_name);

    // Position texture
    m_program->addUniformTexture(3, "emissive_tex");
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, g_buffer_attachments[3]->m_name);

    // Position texture
    m_program->addUniformTexture(4, "normal_tex");
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, g_buffer_attachments[4]->m_name);
}

void Deferred_Pass::set_ssao_attachment(const std::vector<shared_attachment> ao_attachments)
{
    m_program->use();

    // SSAO texture
    m_program->addUniformTexture(5, "ssao_tex");
    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, ao_attachments[1]->m_name);
}

void Deferred_Pass::set_ibl_attachments(const std::vector<shared_attachment> ibl_attachments)
{
    m_program->use();

    // Irradiance Cubemap
    m_program->addUniformTexture(6, "ibl_diffuse");
    glActiveTexture(GL_TEXTURE6);
    glBindTexture(GL_TEXTURE_CUBE_MAP, ibl_attachments[1]->m_name);

    // Specular Cubemap
    m_program->addUniformTexture(7, "ibl_specular");
    glActiveTexture(GL_TEXTURE7);
    glBindTexture(GL_TEXTURE_CUBE_MAP, ibl_attachments[2]->m_name);
}

void Deferred_Pass::set_brdf_lut_attachment(const std::vector<shared_attachment> brdf_lut_attachments)
{
    m_program->use();

    // BRDF Look Up Table
    m_program->addUniformTexture(8, "brdf_lut");
    glActiveTexture(GL_TEXTURE8);
    glBindTexture(GL_TEXTURE_2D, brdf_lut_attachments[0]->m_name);
}