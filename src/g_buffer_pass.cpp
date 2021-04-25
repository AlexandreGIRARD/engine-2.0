#include "g_buffer_pass.hpp"

#include <glad/glad.h>

G_Buffer_Pass::G_Buffer_Pass(unsigned int width, unsigned int height)
    : Render_Pass(true)
{
    // Init program
    m_program->add_shader("deferred/g_buffer.vert", GL_VERTEX_SHADER);
    m_program->add_shader("deferred/g_buffer.frag", GL_FRAGMENT_SHADER);
    m_program->link();

    // Init attachments
    m_attach_position = std::make_shared<Attachment>(width, height, GL_RGBA, GL_FLOAT);
    m_attach_albedo   = std::make_shared<Attachment>(width, height, GL_RGBA, GL_UNSIGNED_BYTE);
    m_attach_orm      = std::make_shared<Attachment>(width, height, GL_RGB, GL_FLOAT);
    m_attach_emissive = std::make_shared<Attachment>(width, height, GL_RGB, GL_UNSIGNED_BYTE);
    m_attach_normal   = std::make_shared<Attachment>(width, height, GL_RGBA, GL_FLOAT);
    m_attach_depth    = std::make_shared<Attachment>(width, height, GL_DEPTH_COMPONENT, GL_FLOAT);

    // Init framebuffer
    m_fbo->bind();
    m_fbo->set_attachment(m_attach_position, GL_COLOR_ATTACHMENT0);
    m_fbo->set_attachment(m_attach_albedo, GL_COLOR_ATTACHMENT1);
    m_fbo->set_attachment(m_attach_orm, GL_COLOR_ATTACHMENT2);
    m_fbo->set_attachment(m_attach_emissive, GL_COLOR_ATTACHMENT3);
    m_fbo->set_attachment(m_attach_normal, GL_COLOR_ATTACHMENT4);
    m_fbo->set_attachment(m_attach_depth, GL_DEPTH_ATTACHMENT);
    m_fbo->unbind();
}

G_Buffer_Pass::~G_Buffer_Pass()
{

}

void G_Buffer_Pass::render(Camera* camera, Scene* scene)
{
    m_program->use();
    glm::mat4 projection_view = camera->get_projection() * camera->look_at();
    glm::mat4 view = camera->look_at();
    m_program->addUniformMat4(projection_view, "projection_view");
    m_program->addUniformMat4(view, "view");
    scene->draw(m_program);
}
