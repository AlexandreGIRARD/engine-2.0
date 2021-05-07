#pragma once

#include "pipeline/screen_pass.hpp"

class Light;
using shared_light = std::shared_ptr<Light>;

namespace pipeline
{
    class Deferred_Pass : public Screen_Pass
    {
    public:
        Deferred_Pass(unsigned int width, unsigned int height);
        ~Deferred_Pass();

        void render(Camera* camera, Scene* scene) override;
        const std::vector<shared_attachment> get_attachments() override;
        void resize(unsigned int width, unsigned int height) override;

        void set_lights(std::vector<shared_light> lights);
        void set_gbuffer_attachments(const std::vector<shared_attachment> g_buffer_attchments);
        void set_ssao_attachment(const std::vector<shared_attachment> ao_attachments);
        void set_ibl_attachments(const std::vector<shared_attachment> ibl_attchments);
        void set_brdf_lut_attachment(const std::vector<shared_attachment> brdf_lut_attachments);

        float* get_ibl_factor() { return &m_ibl_factor; };

        shared_attachment m_attach_output;

    private:
        float m_ibl_factor = 0.2f;
    };
}