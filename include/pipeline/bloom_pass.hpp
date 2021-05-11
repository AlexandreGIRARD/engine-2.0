#pragma once

#include "pipeline/screen_pass.hpp"

namespace pipeline
{
    class Bloom_Pass : public Screen_Pass
    {
    public:
        Bloom_Pass(unsigned int width, unsigned int height);
        ~Bloom_Pass();

        void render(Camera* camera, Scene* scene) override;
        const std::vector<shared_attachment> get_attachments() override;
        void resize(unsigned int width, unsigned int height) override;

        void set_frame_attachments(const std::vector<shared_attachment> g_frame_attachments);

        shared_attachment m_attach_brightness;
        shared_attachment m_attach_bloom1;
        shared_attachment m_attach_bloom2;
        shared_attachment m_attach_bloom3;
        shared_attachment m_attach_bloom1_bis;
        shared_attachment m_attach_bloom2_bis;
        shared_attachment m_attach_bloom3_bis;

        float* get_threshold() { return &m_threshold; }
        float* get_strength() { return &m_strength; }

    private:

        shared_program m_gaussian_program;
        shared_program m_brightness_program;


        float m_threshold = 0.75f;
        float m_strength  = 1.0f;
    };
}