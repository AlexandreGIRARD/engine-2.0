#include "pipeline/bloom_pass.hpp"

#include <glad/glad.h>

using namespace pipeline;

Bloom_Pass::Bloom_Pass(unsigned int width, unsigned int height)
    : Screen_Pass(width, height)
{
    // Init programs
    m_program->add_shader("utils/screen_quad.vert", GL_VERTEX_SHADER);
    m_program->add_shader("fx/blending.frag", GL_FRAGMENT_SHADER);
    m_program->link();

    m_brightness_program = std::make_shared<Program>();
    m_brightness_program->add_shader("utils/screen_quad.vert", GL_VERTEX_SHADER);
    m_brightness_program->add_shader("fx/extract_brightness.frag", GL_FRAGMENT_SHADER);
    m_brightness_program->link();

    m_gaussian_program = std::make_shared<Program>();
    m_gaussian_program->add_shader("utils/screen_quad.vert", GL_VERTEX_SHADER);
    m_gaussian_program->add_shader("fx/gaussian_blur.frag", GL_FRAGMENT_SHADER);
    m_gaussian_program->link();

    // Init Attachment
    m_attach_brightness = std::make_shared<Attachment>(GL_TEXTURE_2D, m_width, m_height, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE);
    m_attach_bloom1     = std::make_shared<Attachment>(GL_TEXTURE_2D, m_width, m_height, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE);
    m_attach_bloom2     = std::make_shared<Attachment>(GL_TEXTURE_2D, m_width / 2, m_height / 2, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE);
    m_attach_bloom3     = std::make_shared<Attachment>(GL_TEXTURE_2D, m_width / 4, m_height / 4, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE);
    m_attach_bloom1_bis = std::make_shared<Attachment>(GL_TEXTURE_2D, m_width, m_height, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE);
    m_attach_bloom2_bis = std::make_shared<Attachment>(GL_TEXTURE_2D, m_width / 2, m_height / 2, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE);
    m_attach_bloom3_bis = std::make_shared<Attachment>(GL_TEXTURE_2D, m_width / 4, m_height / 4, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE);
}

Bloom_Pass::~Bloom_Pass()
{

}


void Bloom_Pass::render(Camera* camera, Scene* scene)
{
    // First Step: Extract Brightness
    m_brightness_program->use();
    m_brightness_program->addUniformFloat(m_threshold, "threshold");
    m_brightness_program->addUniformFloat(m_strength, "strength");

    m_fbo->bind();
    m_fbo->set_attachment(m_attach_brightness, GL_COLOR_ATTACHMENT0);
    render_screen_quad();


    // Second Step: Apply Gaussian blur on multiple scale
    m_gaussian_program->use();
    shared_attachment bloom_attach[] = {m_attach_bloom1, m_attach_bloom2, m_attach_bloom3};
    shared_attachment bloom_attach_bis[] = {m_attach_bloom1_bis, m_attach_bloom2_bis, m_attach_bloom3_bis};
    for (int i = 0; i < 3; i++)
    {
        // Horizontal pass
        m_fbo->bind();
        m_fbo->set_attachment(bloom_attach_bis[0], GL_COLOR_ATTACHMENT0);
        m_program->addUniformTexture(0, "brightness_tex");
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_attach_brightness->m_name);
        m_program->addUniformInt(static_cast<int>(false), "is_horizontal");
        render_screen_quad();

        // vertical pass
        m_fbo->bind();
        m_fbo->set_attachment(bloom_attach[0], GL_COLOR_ATTACHMENT0);
        m_program->addUniformTexture(0, "brightness_tex");
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, bloom_attach_bis[0]->m_name);
        m_program->addUniformInt(static_cast<int>(true), "is_horizontal");
        render_screen_quad();
    }


    // Third Step: Blending texture
}

const std::vector<shared_attachment> Bloom_Pass::get_attachments()
{
    return std::vector{m_attach_brightness, m_attach_bloom1, m_attach_bloom2, m_attach_bloom3};
}

void Bloom_Pass::resize(unsigned int width, unsigned int height)
{
    Render_Pass::resize(width, height);
    m_attach_brightness->resize(width, height);
    m_attach_bloom1->resize(width, height);
    m_attach_bloom2->resize(width / 2, height / 2);
    m_attach_bloom3->resize(width / 4, height / 4);
    m_attach_bloom1_bis->resize(width, height);
    m_attach_bloom2_bis->resize(width / 2, height / 2);
    m_attach_bloom3_bis->resize(width / 4, height / 4);
}


void Bloom_Pass::set_frame_attachments(const std::vector<shared_attachment> g_frame_attachments)
{
    m_program->use();

    m_program->addUniformTexture(0, "frame_tex");
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, g_frame_attachments[0]->m_name);

    m_brightness_program->use();

    m_brightness_program->addUniformTexture(0, "frame_tex");
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, g_frame_attachments[0]->m_name);
}