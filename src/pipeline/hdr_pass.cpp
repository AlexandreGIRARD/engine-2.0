#include "pipeline/hdr_pass.hpp"

#include <glad/glad.h>

using namespace pipeline;

HDR_Pass::HDR_Pass(unsigned int width, unsigned height)
    : Screen_Pass(width, height)
{
    // Init Program
    m_program->add_shader("utils/screen_quad.vert", GL_VERTEX_SHADER);
    m_program->add_shader("fx/hdr.frag", GL_FRAGMENT_SHADER);
    m_program->link();

    // Init attachment
    m_attach_output = std::make_shared<Attachment>(GL_TEXTURE_2D, m_width, m_height, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE);

    m_fbo->bind();
    m_fbo->set_attachment(m_attach_output, GL_COLOR_ATTACHMENT0);
    m_fbo->unbind();
}

HDR_Pass::~HDR_Pass()
{

}


void HDR_Pass::render(Camera* camera, Scene* scene)
{
    m_program->use();
    m_program->addUniformFloat(m_exposure, "exposure");
    m_program->addUniformInt(m_algorithm, "algorithm");

    render_screen_quad();
}

const std::vector<shared_attachment> HDR_Pass::get_attachments()
{
    return  std::vector<shared_attachment>{m_attach_output};
}

void HDR_Pass::resize(unsigned int width, unsigned int height)
{
    Render_Pass::resize(width, height);
    m_attach_output->resize(width, height);
}

void HDR_Pass::set_frame_attachments(const std::vector<shared_attachment> g_frame_attachments)
{
    m_program->use();

    // Position texture
    m_program->addUniformTexture(0, "frame_tex");
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, g_frame_attachments[0]->m_name);
}