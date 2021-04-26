#include "deferred_pass.hpp"

#include <glad/glad.h>

Deferred_Pass::Deferred_Pass(unsigned int width, unsigned int height)
    : Render_Pass(false)    
{
    // Init program
    m_program->add_shader("deferred/deferred.vert", GL_VERTEX_SHADER);
    m_program->add_shader("deferred/deferred.frag", GL_FRAGMENT_SHADER);
    m_program->link();

    // Init attachments
    m_attach_output = std::make_shared<Attachment>(width, height, GL_RGBA, GL_UNSIGNED_BYTE);

    // Init framebuffer
    m_fbo->bind();
    m_fbo->set_attachment(m_attach_output, GL_COLOR_ATTACHMENT0);

    // Init Screen-Quad
    set_screen_quad();
}

Deferred_Pass::~Deferred_Pass()
{
    glDeleteBuffers(1, &m_quad_vao);
    glDeleteVertexArrays(1, &m_quad_vao);
}

const std::vector<shared_attachment> Deferred_Pass::get_attachments()
{
    return std::vector<shared_attachment>{m_attach_output};
}

void Deferred_Pass::set_screen_quad()
{
    float quad[] = {
        -1.f, -1.f, 0.f, 0.f, 0.f,
        -1.f,  1.f, 0.f, 0.f, 1.f,
         1.f, -1.f, 0.f, 1.f, 0.f,
         1.f,  1.f, 0.f, 1.f, 1.f
    };
    glGenVertexArrays(1, &m_quad_vao);
    glGenBuffers(1, &m_quad_vbo);

    glBindVertexArray(m_quad_vao);

    glBindBuffer(GL_ARRAY_BUFFER, m_quad_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad), &quad, GL_STATIC_DRAW);

    // Positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)0);

    // Uvs
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)(3 * sizeof(float)));
}

void Deferred_Pass::render_screen_quad()
{
    // Framebuffer Binding Points
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo->get_name());
    glDrawBuffer(GL_COLOR_ATTACHMENT0);
    
    // Disable depth test
    glDisable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT);

    // Render call
    glBindVertexArray(m_quad_vao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);

    // Restore state
    glEnable(GL_DEPTH_TEST);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Deferred_Pass::render(Camera* camera, Scene* scene)
{
    m_program->use();
    glm::mat4 projection_view = camera->get_projection() * camera->look_at();
    glm::mat4 view = camera->look_at();
    m_program->addUniformMat4(projection_view, "projection_view");
    render_screen_quad();
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

void Deferred_Pass::set_ssao_attachment(const shared_attachment ssao_attachment)
{
    m_program->use();

    // SSAO texture
    m_program->addUniformTexture(5, "ssao_tex");
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, ssao_attachment->m_name);
}