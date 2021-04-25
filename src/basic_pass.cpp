#include "basic_pass.hpp"

#include <glad/glad.h>

Basic_Pass::Basic_Pass()
:  Render_Pass(true)
{
    m_program->add_shader("basic/basic.vert", GL_VERTEX_SHADER);
    m_program->add_shader("basic/basic.frag", GL_FRAGMENT_SHADER);
    m_program->link();
}

Basic_Pass::~Basic_Pass()
{}

void Basic_Pass::render(Camera* camera, Scene* scene)
{
    m_program->use();
    glm::mat4 projection_view = camera->get_projection() * camera->look_at();
    m_program->addUniformMat4(projection_view, "projection_view");
    scene->draw(m_program);
}
