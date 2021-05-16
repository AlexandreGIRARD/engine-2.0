#include "pipeline/aa_pass.hpp"

#include <glad/glad.h>

using namespace pipeline;

AA_Pass::AA_Pass(unsigned int width, unsigned int height)
    : Screen_Pass(width, height)
{
    // Init program
    m_program->add_shader("utils/screen_quad.vert", GL_VERTEX_SHADER);
    m_program->add_shader("fx/fxaa.frag", GL_FRAGMENT_SHADER);
    m_program->link();

    // Init Attachment
    m_attach_output = std::make_shared<Attachment>(GL_TEXTURE_2D, m_width, m_height, GL_RGB16F, GL_RGB, GL_FLOAT);

    // Init Framebuffer
    m_fbo->bind();
    m_fbo->set_attachment(m_attach_output, GL_COLOR_ATTACHMENT0);
    m_fbo->unbind();
}

AA_Pass::~AA_Pass()
{

}

const std::vector<shared_attachment> AA_Pass::get_attachments()
{
    return std::vector<shared_attachment>{m_attach_output};
}

void AA_Pass::resize(unsigned int width, unsigned int height)
{
    Render_Pass::resize(width, height);
    m_attach_output->resize(width, height);
}

void AA_Pass::render(Camera* camera, Scene* scene)
{
    m_program->use();
    m_program->addUniformVec2(glm::vec2{1.f / m_width, 1.f / m_height}, "inv_scren_size");
    m_program->addUniformInt(static_cast<int>(m_is_debug), "is_debug");

    render_screen_quad();
}

void AA_Pass::set_frame_attachments(const std::vector<shared_attachment> g_frame_attachments)
{
    m_program->use();
    m_program->addUniformTexture2D(g_frame_attachments[0]->m_name, 0, "frame_tex"); // Frame texture
}