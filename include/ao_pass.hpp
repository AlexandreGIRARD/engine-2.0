#pragma once

#include "screen_pass.hpp"

#include <glm/glm.hpp>

// http://john-chapman-graphics.blogspot.com/2013/01/ssao-tutorial.html
// https://learnopengl.com/Advanced-Lighting/SSAO
class AO_Pass : public Screen_Pass
{
public:
    AO_Pass(unsigned int width, unsigned int height);
    ~AO_Pass();

    void render(Camera* camera, Scene* scene) override;
    const std::vector<shared_attachment> get_attachments() override;
    void resize(unsigned int width, unsigned int height) override;

    void set_gbuffer_attachments(const std::vector<shared_attachment> g_buffer_attchments);

    shared_attachment m_attach_ao;
    shared_attachment m_attach_blured;

    float* get_radius() { return &m_radius; }
    float* get_power() { return &m_power; }

private:
    void init_samples();
    void init_noise();

    shared_program m_blur_program;

    std::vector<glm::vec3> m_samples; // 64 samples vector
    Texture* m_noise_tex; // 4x4 Noise texture

    float m_radius = 0.01f;
    float m_power  = 1.f;
};