#include "brdf_lut_pass.hpp"

#include <glad/glad.h>

BRDF_LUT_Pass::BRDF_LUT_Pass(unsigned int width, unsigned int height)
    : Screen_Pass(width, height)
{
    m_program->add_shader("brdf_lut/generate.vert", GL_VERTEX_SHADER);
    m_program->add_shader("brdf_lut/generate.frag", GL_FRAGMENT_SHADER);
    m_program->link();

    m_attach_lut = std::make_shared<Attachment>(GL_TEXTURE_2D, 512, 512, GL_RG16F, GL_RG, GL_FLOAT);

    m_fbo->bind();
    m_fbo->set_attachment(m_attach_lut, GL_COLOR_ATTACHMENT0);

}

BRDF_LUT_Pass::~BRDF_LUT_Pass()
{
}

void BRDF_LUT_Pass::render(Camera* camera, Scene* scene)
{
    m_program->use();

    glViewport(0, 0, 512, 512);

    render_screen_quad();

    glViewport(0, 0, m_width, m_height);
}

const std::vector<shared_attachment> BRDF_LUT_Pass::get_attachments()
{
    return std::vector<shared_attachment>{m_attach_lut};
}

void BRDF_LUT_Pass::resize(unsigned int width, unsigned int height)
{
    // Osef ?
}
