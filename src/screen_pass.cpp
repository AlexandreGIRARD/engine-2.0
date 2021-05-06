#include "screen_pass.hpp"

#include <glad/glad.h>

Screen_Pass::Screen_Pass(unsigned int width, unsigned int height)
    : Render_Pass(width, height, false)
{
    set_screen_quad();
}

void Screen_Pass::set_screen_quad()
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

void Screen_Pass::render_screen_quad()
{
    // Framebuffer Binding Points
    m_fbo->bind();
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
    m_fbo->unbind();
}