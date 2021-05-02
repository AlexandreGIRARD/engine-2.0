#include "skybox_pass.hpp"

#include <glad/glad.h>

Skybox_Pass::Skybox_Pass(unsigned int width, unsigned int height)
    : Render_Pass(width, height, false)
{
    // Init Skybor Rendering program
    m_program->add_shader("envmap/skybox.vert", GL_VERTEX_SHADER);
    m_program->add_shader("envmap/skybox.frag", GL_FRAGMENT_SHADER);
    m_program->link();

    m_attach_color = std::make_shared<Attachment>(GL_TEXTURE_2D, m_width, m_height, GL_RGBA, GL_UNSIGNED_BYTE);
    m_attach_depth = std::make_shared<Attachment>(GL_TEXTURE_2D, m_width, m_height, GL_DEPTH_COMPONENT, GL_FLOAT);
    
    set_cube();
}

void Skybox_Pass::resize(unsigned int width, unsigned int height)
{
    Render_Pass::resize(width, height);
    m_attach_color->resize(width, height);
    m_attach_depth->resize(width, height);
}

void Skybox_Pass::set_cube()
{
    const float skybox_cube[] = {
        // positions
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    glGenVertexArrays(1, &m_cube_vao);
    glGenBuffers(1, &m_cube_vbo);

    glBindVertexArray(m_cube_vao);

    glBindBuffer(GL_ARRAY_BUFFER, m_cube_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skybox_cube), &skybox_cube, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
}

void Skybox_Pass::render(Camera* camera, Scene* scene)
{
    m_fbo->bind();
    m_program->use();
    glDepthFunc(GL_LEQUAL);

    // Binding uniforms
    glm::mat4 projection_view = camera->get_projection() * glm::mat4(glm::mat3(camera->look_at()));
    m_program->addUniformMat4(projection_view, "projection_view");
    
    glDrawBuffer(GL_COLOR_ATTACHMENT0);

    glBindVertexArray(m_cube_vao);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    glDepthFunc(GL_LESS);
    m_fbo->unbind();
}

void Skybox_Pass::blit_buffers(const shared_fbo color_fbo, const shared_fbo depth_fbo)
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo->get_name());
    m_fbo->set_attachment(m_attach_color, GL_COLOR_ATTACHMENT0);
    m_fbo->set_attachment(m_attach_depth, GL_DEPTH_ATTACHMENT);

    // Copy color buffer
    glBindFramebuffer(GL_READ_FRAMEBUFFER, color_fbo->get_name());
    glBlitFramebuffer(0, 0, m_width, m_height, 0, 0, m_width, m_height, GL_COLOR_BUFFER_BIT, GL_LINEAR);

    // Copy depth buffer
    glBindFramebuffer(GL_READ_FRAMEBUFFER, depth_fbo->get_name());
    glBlitFramebuffer(0, 0, m_width, m_height, 0, 0, m_width, m_height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Skybox_Pass::set_skybox_attachments(const std::vector<shared_attachment> attachments)
{
    m_program->use();
    // Skybox Cubemap binding
    m_program->addUniformTexture(0, "skybox");
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, attachments[0]->m_name);
}

const std::vector<shared_attachment> Skybox_Pass::get_attachments()
 {
     return std::vector<shared_attachment>{m_attach_color, m_attach_depth};
 }