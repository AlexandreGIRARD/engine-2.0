#pragma once

#include "pipeline/screen_pass.hpp"

namespace pipeline
{
    class DOF_Pass : public Screen_Pass
    {
    public:
        DOF_Pass(unsigned int width, unsigned int height);
        ~DOF_Pass();

        void render(Camera* camera, Scene* scene) override;
        const std::vector<shared_attachment> get_attachments() override;
        void resize(unsigned int width, unsigned int height) override;

        void set_gbuffer_attachments(const std::vector<shared_attachment> gbuffer_attachments);
        void set_frame_attachments(const std::vector<shared_attachment> frame_attachments);

        shared_attachment m_attach_output;
        shared_attachment m_attach_factor;
        shared_attachment m_attach_blur_first;
        shared_attachment m_attach_blur_second;

        float* get_diameter() { return &m_diameter; }
        float* get_focal_length() { return &m_focal_length; }
        float* get_zfocus() { return &m_zfocus; }
        float* get_zrange() { return &m_zrange; }

    private:

        shared_program m_depth_program;
        shared_program m_blur_program;

        unsigned int m_frame_name; // Frame attachment id

        float m_diameter = 0.2;
        float m_focal_length = 0.1;
        float m_zfocus = 2.f;
        float m_zrange = 0.5f;
    };
}