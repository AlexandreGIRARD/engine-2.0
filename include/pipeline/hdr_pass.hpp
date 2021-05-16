#pragma once

#include "pipeline/screen_pass.hpp"

namespace pipeline
{
    class HDR_Pass : public Screen_Pass
    {
    public:
        HDR_Pass(unsigned int width, unsigned int height);
        ~HDR_Pass();

        void render(Camera* camera, Scene* scene) override;
        const std::vector<shared_attachment> get_attachments() override;
        void resize(unsigned int width, unsigned int height) override;

        void set_frame_attachments(const std::vector<shared_attachment> g_frame_attachments);

        shared_attachment m_attach_output;

        float* get_exposure() { return &m_exposure; }
        float* get_gamma() { return &m_gamma; }
        int* get_algorithm() { return &m_algorithm; }

    private:
        float m_exposure = 1.f;
        float m_gamma = 2.2f;
        int m_algorithm;
    };
}