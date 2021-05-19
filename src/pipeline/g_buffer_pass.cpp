#include "pipeline/g_buffer_pass.hpp"

#include <glad/glad.h>

using namespace pipeline;

G_Buffer_Pass::G_Buffer_Pass(unsigned int width, unsigned int height)
    : Render_Pass(width, height, true)
{
    // Init program
    m_program->add_shader("deferred/g_buffer.vert", GL_VERTEX_SHADER);
    m_program->add_shader("deferred/g_buffer.frag", GL_FRAGMENT_SHADER);
    m_program->link();

    // Init attachments
    m_attach_position   = std::make_shared<Attachment>(GL_TEXTURE_2D, m_width, m_height, GL_RGBA16F, GL_RGBA, GL_FLOAT);
    m_attach_base_color = std::make_shared<Attachment>(GL_TEXTURE_2D, m_width, m_height, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE);
    m_attach_orm        = std::make_shared<Attachment>(GL_TEXTURE_2D, m_width, m_height, GL_RGBA, GL_RGBA, GL_FLOAT);
    m_attach_emissive   = std::make_shared<Attachment>(GL_TEXTURE_2D, m_width, m_height, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE);
    m_attach_normal     = std::make_shared<Attachment>(GL_TEXTURE_2D, m_width, m_height, GL_RGB16F, GL_RGB, GL_FLOAT);
    m_attach_depth      = std::make_shared<Attachment>(GL_TEXTURE_2D, m_width, m_height, GL_DEPTH_COMPONENT16, GL_DEPTH_COMPONENT, GL_FLOAT);

    // Init framebuffer
    m_fbo->bind();
    m_fbo->set_attachment(m_attach_position, GL_COLOR_ATTACHMENT0);
    m_fbo->set_attachment(m_attach_base_color, GL_COLOR_ATTACHMENT1);
    m_fbo->set_attachment(m_attach_orm, GL_COLOR_ATTACHMENT2);
    m_fbo->set_attachment(m_attach_emissive, GL_COLOR_ATTACHMENT3);
    m_fbo->set_attachment(m_attach_normal, GL_COLOR_ATTACHMENT4);
    m_fbo->set_attachment(m_attach_depth, GL_DEPTH_ATTACHMENT);
    m_fbo->unbind();

    m_buffers = new unsigned int[5]{GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4};
}

G_Buffer_Pass::~G_Buffer_Pass()
{
    delete[] m_buffers;
}

 const std::vector<shared_attachment> G_Buffer_Pass::get_attachments()
 {
     return std::vector<shared_attachment>{m_attach_position, m_attach_base_color, m_attach_orm, m_attach_emissive, m_attach_normal, m_attach_depth};
 }

 void G_Buffer_Pass::resize(unsigned int width, unsigned int height)
 {
    Render_Pass::resize(width, height);
    m_attach_position->resize(width, height);
    m_attach_base_color->resize(width, height);
    m_attach_orm->resize(width, height);
    m_attach_emissive->resize(width, height);
    m_attach_normal->resize(width, height);
    m_attach_depth->resize(width, height);
 }

void G_Buffer_Pass::render(Camera* camera, Scene* scene)
{
    m_program->use();
    glm::mat4 projection_view = camera->get_projection() * camera->look_at();
    glm::mat4 view = camera->look_at();
    m_program->addUniformMat4(projection_view, "projection_view");

    // Framebuffer binding points    
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo->get_name());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDrawBuffers(5, m_buffers);

    scene->draw(m_program);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
