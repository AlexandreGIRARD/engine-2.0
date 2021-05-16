#include "pipeline/dof_pass.hpp"

#include <glad/glad.h>

using namespace pipeline;

DOF_Pass::DOF_Pass(unsigned int width, unsigned height)
    : Screen_Pass(width, height)
{
    // Init Program
    // m_program->add_shader("utils/screen_quad.vert", GL_VERTEX_SHADER);
    // m_program->add_shader("fx/dof.frag", GL_FRAGMENT_SHADER);
    // m_program->link();

    // m_blur_program = std::make_shared<Program>();
    // m_blur_program->add_shader("utils/screen_quad.vert", GL_VERTEX_SHADER);
    // m_blur_program->add_shader("fx/gaussian_blur.frag", GL_FRAGMENT_SHADER);
    // m_blur_program->link();

    m_depth_program = std::make_shared<Program>();
    m_depth_program->add_shader("utils/screen_quad.vert", GL_VERTEX_SHADER);
    m_depth_program->add_shader("fx/dof_depth.frag", GL_FRAGMENT_SHADER);
    m_depth_program->link();

    // Init attachment
    m_attach_output = std::make_shared<Attachment>(GL_TEXTURE_2D, m_width, m_height, GL_RGB16F, GL_RGB, GL_FLOAT);
    m_attach_depth  = std::make_shared<Attachment>(GL_TEXTURE_2D, m_width, m_height, GL_R16F, GL_RED, GL_FLOAT);
    m_attach_blur   = std::make_shared<Attachment>(GL_TEXTURE_2D, m_width / 2.f, m_height / 2.f, GL_RGB16F, GL_RGB, GL_FLOAT);

    m_fbo->bind();
    m_fbo->set_attachment(m_attach_depth, GL_COLOR_ATTACHMENT0);
    m_fbo->unbind();
}

DOF_Pass::~DOF_Pass()
{

}


void DOF_Pass::render(Camera* camera, Scene* scene)
{
    m_depth_program->use();
    m_depth_program->addUniformFloat(m_diameter, "diameter");
    m_depth_program->addUniformFloat(m_focal_length, "focal_length");
    m_depth_program->addUniformFloat(m_zfocus, "z_focus");
    m_depth_program->addUniformFloat(m_zrange, "z_range");
    m_depth_program->addUniformMat4(camera->look_at(), "view");

    render_screen_quad(std::vector{m_attach_depth});

    m_blur_program->use();

    render_screen_quad(std::vector{m_attach_blur});

    m_program->use();

    render_screen_quad(std::vector{m_attach_output});
}

const std::vector<shared_attachment> DOF_Pass::get_attachments()
{
    return  std::vector<shared_attachment>{m_attach_output, m_attach_depth, m_attach_blur};
}

void DOF_Pass::resize(unsigned int width, unsigned int height)
{
    Render_Pass::resize(width, height);
    m_attach_output->resize(width, height);
    m_attach_depth->resize(width, height);
    m_attach_blur->resize(width / 2.f, height / 2.f);
}

void DOF_Pass::set_gbuffer_attachments(const std::vector<shared_attachment> gbuffer_attachments)
{
    m_depth_program->use();
    m_depth_program->addUniformTexture2D(gbuffer_attachments[0]->m_name, 0, "position_tex"); // Position texture
}

void DOF_Pass::set_frame_attachments(const std::vector<shared_attachment> frame_attachments)
{
    m_program->use();
    m_depth_program->addUniformTexture2D(frame_attachments[0]->m_name, 0, "frame_tex"); // Frame texture
}