#include "ao_pass.hpp"

#include <random>
#include <glad/glad.h>
#include <glm/gtx/compatibility.hpp>

#include "texture.hpp"

#define KERNEL_SIZE 64

AO_Pass::AO_Pass(unsigned int width, unsigned int height)
    : Screen_Pass(width, height)    
{
    // Init program
    m_program->add_shader("ambient_occlusion/ssao.vert", GL_VERTEX_SHADER);
    m_program->add_shader("ambient_occlusion/ssao.frag", GL_FRAGMENT_SHADER);
    m_program->link();

    // Init blur program
    m_blur_program = std::make_shared<Program>();
    m_blur_program->add_shader("ambient_occlusion/ssao.vert", GL_VERTEX_SHADER);
    m_blur_program->add_shader("ambient_occlusion/blur.frag", GL_FRAGMENT_SHADER);
    m_blur_program->link();   

    // Init attachments
    m_attach_ao     = std::make_shared<Attachment>(GL_TEXTURE_2D, m_width, m_height, GL_R16F, GL_RED, GL_FLOAT);
    m_attach_blured = std::make_shared<Attachment>(GL_TEXTURE_2D, m_width, m_height, GL_R16F, GL_RED, GL_FLOAT);

    init_samples();
    init_noise();
}

AO_Pass::~AO_Pass()
{
    delete m_noise_tex;
}

const std::vector<shared_attachment> AO_Pass::get_attachments()
{
    return std::vector<shared_attachment>{m_attach_ao, m_attach_blured};
}

void AO_Pass::resize(unsigned int width, unsigned int height)
{
    Render_Pass::resize(width, height);
    m_attach_ao->resize(width, height);
    m_attach_blured->resize(width, height);
}

void AO_Pass::render(Camera* camera, Scene* scene)
{
    // First pass: Occlusion Pass
    m_program->use();
    m_program->addUniformMat4(camera->get_projection(), "projection");
    m_program->addUniformMat4(camera->look_at(), "view");
    m_program->addUniformVec2(glm::vec2{m_width / 4.0, m_height / 4.0}, "noise_scale");

    for (int i = 0; i < KERNEL_SIZE; i++)
        m_program->addUniformVec3(m_samples[i], ("samples[" + std::to_string(i) + "]").c_str());
    m_program->addUniformFloat(m_radius, "radius");
    m_program->addUniformFloat(m_power, "power");
    m_noise_tex->bind(m_program, 3, "noise_tex");

    m_fbo->bind();
    m_fbo->set_attachment(m_attach_ao, GL_COLOR_ATTACHMENT0);

    render_screen_quad();

    // Second pass: Blur Pass
    m_blur_program->use();
    // To blur texture
    m_program->addUniformTexture(0, "ao_tex");
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_attach_ao->m_name);

    m_fbo->bind();
    m_fbo->set_attachment(m_attach_blured, GL_COLOR_ATTACHMENT0);
    render_screen_quad();
}

void AO_Pass::set_gbuffer_attachments(const std::vector<shared_attachment> gbuffer_attachments)
{
    m_program->use();

    // Position texture
    m_program->addUniformTexture(0, "position_tex");
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gbuffer_attachments[0]->m_name);

    // Normal texture
    m_program->addUniformTexture(1, "normal_tex");
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, gbuffer_attachments[4]->m_name);
}

void AO_Pass::init_samples()
{
    std::uniform_real_distribution<float> dis1(-1.f, 1.f);
    std::uniform_real_distribution<float> dis2(-1.f, 1.f);
    std::random_device gen;

    // Generate samples
    m_samples.reserve(KERNEL_SIZE);
    for (int i = 0; i < KERNEL_SIZE; i++)
    {
        m_samples[i] = glm::vec3{dis1(gen), dis1(gen), dis2(gen)};
        glm::normalize(m_samples[i]);

        float scale = float(i) / float(KERNEL_SIZE);
        scale = glm::lerp(0.1f, 1.0f, scale * scale);
        m_samples[i] *= scale;
    }
}

void AO_Pass::init_noise()
{
    std::uniform_real_distribution<float> dis(-1.f, 1.f);
    std::random_device gen;

    // Generate noise texture
    glm::vec3* noise = new glm::vec3[16];

    for (int i = 0; i < 16; ++i)
        noise[i] = glm::normalize(glm::vec3{dis(gen), dis(gen), 0});
    
    Texture_Info infos{
        4,
        4,
        GL_REPEAT,
        GL_REPEAT,
        GL_NEAREST,
        GL_NEAREST,
        GL_RGB16F,
        GL_RGB,
        GL_FLOAT
    };

    m_noise_tex = new Texture(infos, reinterpret_cast<char*>(noise));
    delete[] noise;
}