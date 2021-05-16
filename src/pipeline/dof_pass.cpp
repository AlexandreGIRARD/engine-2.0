#include "pipeline/dof_pass.hpp"

#include <glad/glad.h>

using namespace pipeline;

DOF_Pass::DOF_Pass(unsigned int width, unsigned height)
    : Screen_Pass(width, height)
{
    // Init Program
    m_program->add_shader("utils/screen_quad.vert", GL_VERTEX_SHADER);
    m_program->add_shader("fx/dof.frag", GL_FRAGMENT_SHADER);
    m_program->link();

    m_blur_program = std::make_shared<Program>();
    m_blur_program->add_shader("utils/screen_quad.vert", GL_VERTEX_SHADER);
    m_blur_program->add_shader("fx/gaussian_blur.frag", GL_FRAGMENT_SHADER);
    m_blur_program->link();

    m_depth_program = std::make_shared<Program>();
    m_depth_program->add_shader("utils/screen_quad.vert", GL_VERTEX_SHADER);
    m_depth_program->add_shader("fx/dof_depth.frag", GL_FRAGMENT_SHADER);
    m_depth_program->link();

    // Init attachment
    m_attach_output = std::make_shared<Attachment>(GL_TEXTURE_2D, m_width, m_height, GL_RGB16F, GL_RGB, GL_FLOAT);
    m_attach_factor  = std::make_shared<Attachment>(GL_TEXTURE_2D, m_width, m_height, GL_RED, GL_RED, GL_FLOAT);
    m_attach_blur_first = std::make_shared<Attachment>(GL_TEXTURE_2D, m_width / 2.f, m_height / 2.f, GL_RGB16F, GL_RGB, GL_FLOAT);
    m_attach_blur_second= std::make_shared<Attachment>(GL_TEXTURE_2D, m_width / 2.f, m_height / 2.f, GL_RGB16F, GL_RGB, GL_FLOAT);

    m_fbo->bind();
    m_fbo->set_attachment(m_attach_factor, GL_COLOR_ATTACHMENT0);
    m_fbo->unbind();
}

DOF_Pass::~DOF_Pass()
{

}


void DOF_Pass::render(Camera* camera, Scene* scene)
{
    // First pass: compute factor
    m_depth_program->use();
    m_depth_program->addUniformFloat(m_focal_length / m_diameter, "aperture");
    m_depth_program->addUniformFloat(m_focal_length, "focal_length");
    m_depth_program->addUniformFloat(m_zfocus, "z_focus");
    m_depth_program->addUniformMat4(camera->look_at(), "view");

    render_screen_quad(std::vector{m_attach_factor});

    // Second pass: blur the frame
    glViewport(0, 0, m_width / 2.f, m_height / 2.f);
    m_blur_program->use();

    // Horizontal pass
    m_blur_program->addUniformTexture2D(m_frame_name, 0, "to_blur_tex");
    m_blur_program->addUniformInt(1, "is_horizontal");
    render_screen_quad(std::vector{m_attach_blur_first});

    // Vertical pass
    m_blur_program->addUniformTexture2D(m_attach_blur_first->m_name, 0, "to_blur_tex");
    m_blur_program->addUniformInt(0, "is_horizontal");
    render_screen_quad(std::vector{m_attach_blur_second});

    glViewport(0, 0, m_width, m_height);

    // Third pass: dompute depth of field
    m_program->use();
    m_program->addUniformTexture2D(m_frame_name, 0, "frame_tex");
    m_program->addUniformTexture2D(m_attach_factor->m_name, 1, "factor_tex");
    m_program->addUniformTexture2D(m_attach_blur_first->m_name, 2, "blured_tex");

    render_screen_quad(std::vector{m_attach_output});
}

const std::vector<shared_attachment> DOF_Pass::get_attachments()
{
    return  std::vector<shared_attachment>{m_attach_output, m_attach_factor, m_attach_blur_first, m_attach_blur_second};
}

void DOF_Pass::resize(unsigned int width, unsigned int height)
{
    Render_Pass::resize(width, height);
    m_attach_output->resize(width, height);
    m_attach_factor->resize(width, height);
    m_attach_blur_first->resize(width / 2.f, height / 2.f);
    m_attach_blur_second->resize(width / 2.f, height / 2.f);
}

void DOF_Pass::set_gbuffer_attachments(const std::vector<shared_attachment> gbuffer_attachments)
{
    m_depth_program->use();
    m_depth_program->addUniformTexture2D(gbuffer_attachments[0]->m_name, 0, "position_tex"); // Position texture
}

void DOF_Pass::set_frame_attachments(const std::vector<shared_attachment> frame_attachments)
{
    m_frame_name = frame_attachments[0]->m_name;
}