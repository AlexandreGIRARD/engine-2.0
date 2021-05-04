#include "brdf_lut_pass.hpp"

#include <glad/glad.h>

BRDF_LUT_Pass::BRDF_LUT_Pass(unsigned int width, unsigned int height)
    : Render_Pass(width, height, false)
{
    m_program->add_shader("brdf_lut/generate.vert", GL_VERTEX_SHADER);
    m_program->add_shader("brdf_lut/generate.frag", GL_FRAGMENT_SHADER);
    m_program->link();

    m_attach_lut = std::make_shared<Attachment>(GL_TEXTURE_2D, 512, 512, GL_RG, GL_FLOAT);

    m_fbo->bind();
    m_fbo->set_attachment(m_attach_lut, GL_COLOR_ATTACHMENT0);

    set_screen_quad();
}

BRDF_LUT_Pass::~BRDF_LUT_Pass()
{
    m_attach_lut  = std::make_shared<Attachment>(GL_TEXTURE_2D, 512, 512, GL_RG, GL_FLOAT);
}

void BRDF_LUT_Pass::render(Camera* camera, Scene* scene)
{
    m_program->use();

    m_fbo->bind();
    glViewport(0, 0, 512, 512);
    // Disable depth test
    glDisable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT);

    // Render call
    glBindVertexArray(m_quad_vao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);

    // Restore state
    glEnable(GL_DEPTH_TEST);
    m_fbo->unbind();
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

void BRDF_LUT_Pass::set_screen_quad()
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