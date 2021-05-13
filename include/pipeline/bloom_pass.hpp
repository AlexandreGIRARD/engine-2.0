#pragma once

#include "pipeline/screen_pass.hpp"

#define BLUR_LEVELS 4 

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
        shared_attachment m_attach_bloom_first[BLUR_LEVELS];
        shared_attachment m_attach_bloom_second[BLUR_LEVELS];
        shared_attachment m_attach_output;

        float* get_threshold() { return &m_threshold; }
        float* get_strength() { return &m_strength; }

    private:

        shared_program m_gaussian_program;
        shared_program m_brightness_program;

        unsigned int m_frame_name;

        float m_threshold = 0.5f;
        float m_strength  = 1.0f;
    };
}