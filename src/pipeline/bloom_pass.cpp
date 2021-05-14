#include "pipeline/bloom_pass.hpp"
#include <iostream>
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
    m_attach_brightness    = std::make_shared<Attachment>(GL_TEXTURE_2D, m_width, m_height, GL_RGB16F, GL_RGB, GL_FLOAT);
    
    for (int level = 0; level < BLUR_LEVELS; level++)
    {
        float coef = std::pow(0.5, level);
        m_attach_bloom_first[level]  = std::make_shared<Attachment>(GL_TEXTURE_2D, m_width * coef, m_height * coef, GL_RGB16F, GL_RGB, GL_FLOAT);
        m_attach_bloom_second[level] = std::make_shared<Attachment>(GL_TEXTURE_2D, m_width * coef, m_height * coef, GL_RGB16F, GL_RGB, GL_FLOAT);
    }
    m_attach_output       = std::make_shared<Attachment>(GL_TEXTURE_2D, m_width, m_height, GL_RGB16F, GL_RGB, GL_FLOAT);
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


    glViewport(0, 0, m_width, m_height);
    m_fbo->bind();
    m_fbo->set_attachment(m_attach_brightness, GL_COLOR_ATTACHMENT0);
    render_screen_quad();
    m_attach_brightness->generate_mipmap();

    // EROROR
    unsigned int error;

    // Second Step: Apply Gaussian blur on multiple scale
    m_gaussian_program->use();
    for (int level = 0; level < BLUR_LEVELS; level++)
    {
        glViewport(0, 0, m_width * std::pow(0.5, level), m_height * std::pow(0.5, level));
        m_gaussian_program->addUniformInt(level, "mip_level");

        // Horizontal pass
        m_fbo->bind();
        m_fbo->set_attachment(m_attach_bloom_first[level], GL_COLOR_ATTACHMENT0);
        m_gaussian_program->addUniformTexture(0, "brightness_tex");
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_attach_brightness->m_name);
        m_gaussian_program->addUniformInt(1, "is_horizontal");
        render_screen_quad();

        // Vertical pass
        m_fbo->bind();
        m_fbo->set_attachment(m_attach_bloom_second[level], GL_COLOR_ATTACHMENT0);
        m_gaussian_program->addUniformTexture(0, "brightness_tex");
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_attach_bloom_first[level]->m_name);
        m_gaussian_program->addUniformInt(0, "is_horizontal");
        render_screen_quad();
    }

    // Third Step: Blending texture
    m_program->use();
    glViewport(0, 0, m_width, m_height);
    // Set texture input
    m_program->addUniformTexture(0, "frame_tex");
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_frame_name);

    for (int level = 0; level < BLUR_LEVELS; level++)
    {
        m_program->addUniformTexture(level + 1, ("bloom_tex[" + std::to_string(level) + "]").c_str());
        glActiveTexture(GL_TEXTURE1 + level);
        glBindTexture(GL_TEXTURE_2D, m_attach_bloom_second[level]->m_name);
    }

    m_fbo->bind();
    m_fbo->set_attachment(m_attach_output, GL_COLOR_ATTACHMENT0);
    render_screen_quad();
    
}

const std::vector<shared_attachment> Bloom_Pass::get_attachments()
{
    return std::vector{m_attach_output, m_attach_brightness};
}

void Bloom_Pass::resize(unsigned int width, unsigned int height)
{
    Render_Pass::resize(width, height);
    m_attach_brightness->resize(width, height);
    for (int level = 0; level < BLUR_LEVELS; level++)
    {
        float coef = std::pow(0.5, level);
        m_attach_bloom_first[level]->resize(m_width * coef, m_height * coef);
        m_attach_bloom_second[level]->resize(m_width * coef, m_height * coef);
    }
    m_attach_output->resize(width, height);
}


void Bloom_Pass::set_frame_attachments(const std::vector<shared_attachment> g_frame_attachments)
{
    m_frame_name = g_frame_attachments[0]->m_name;
    m_brightness_program->use();

    m_brightness_program->addUniformTexture(0, "frame_tex");
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, g_frame_attachments[0]->m_name);
}