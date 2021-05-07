#include "pipeline/debug_pass.hpp"

#include <glad/glad.h>

using namespace pipeline;

Debug_Pass::Debug_Pass(unsigned int width, unsigned int height)
    : Screen_Pass(width, height)
{
    // Init Program
    m_program->add_shader("utils/screen_quad.vert", GL_VERTEX_SHADER);
    m_program->add_shader("utils/debug.frag", GL_FRAGMENT_SHADER);
    m_program->link();

    // Init attachment
    m_attach_output = std::make_shared<Attachment>(GL_TEXTURE_2D, m_width, m_height, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE);

    m_fbo->bind();
    m_fbo->set_attachment(m_attach_output, GL_COLOR_ATTACHMENT0);
    m_fbo->unbind();
}

Debug_Pass::~Debug_Pass()
{
    // Nothing to delete 
}

void Debug_Pass::render(Camera* camera, Scene* scene)
{
    m_program->use();

    render_screen_quad();
}

const std::vector<shared_attachment> Debug_Pass::get_attachments()
{
    return std::vector<shared_attachment>{m_attach_output};
}

void Debug_Pass::resize(unsigned int width, unsigned int height)
{
    Render_Pass::resize(width, height);
    m_attach_output->resize(width, height);
}

void Debug_Pass::set_attachment(const shared_attachment attachment, const int mode)
{
    m_program->use();
    
    // Debug texture
    m_program->addUniformTexture(0, "debug_tex");
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, attachment->m_name);

    m_program->addUniformInt(mode, "mode");
}
