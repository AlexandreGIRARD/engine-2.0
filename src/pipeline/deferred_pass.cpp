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
    m_attach_output = std::make_shared<Attachment>(GL_TEXTURE_2D, m_width, m_height, GL_RGB16F, GL_RGB, GL_FLOAT);

    // Init framebuffer
    m_fbo->bind();
    m_fbo->set_attachment(m_attach_output, GL_COLOR_ATTACHMENT0);
    m_fbo->unbind();
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
    m_program->addUniformTexture2D(g_buffer_attachments[0]->m_name, 0, "position_tex"); // Position texture
    m_program->addUniformTexture2D(g_buffer_attachments[1]->m_name, 1, "base_color_tex"); // Base Color texture
    m_program->addUniformTexture2D(g_buffer_attachments[2]->m_name, 2, "orm_tex"); // Position texture
    m_program->addUniformTexture2D(g_buffer_attachments[30]->m_name, 3, "emissive_tex"); // Position texture
    m_program->addUniformTexture2D(g_buffer_attachments[4]->m_name, 4, "normal_tex");} // Position texture

void Deferred_Pass::set_ssao_attachment(const std::vector<shared_attachment> ao_attachments)
{
    m_program->use();
    m_program->addUniformTexture2D(ao_attachments[1]->m_name, 5, "ssao_tex"); // AO texture
}

void Deferred_Pass::set_ibl_attachments(const std::vector<shared_attachment> ibl_attachments)
{
    m_program->use();
    m_program->addUniformTextureCubeMap(ibl_attachments[1]->m_name, 6, "ibl_diffuse"); // Irradiance Cubemap
    m_program->addUniformTextureCubeMap(ibl_attachments[2]->m_name, 7, "ibl_specular"); // Specular Cubemap
}

void Deferred_Pass::set_brdf_lut_attachment(const std::vector<shared_attachment> brdf_lut_attachments)
{
    m_program->use();
    m_program->addUniformTexture2D(brdf_lut_attachments[0]->m_name, 8, "brdf_lut"); // BRDF Look Up Table
}