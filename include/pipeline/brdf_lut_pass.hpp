#pragma once

#include "pipeline/screen_pass.hpp"

namespace pipeline
{
    class BRDF_LUT_Pass : public Screen_Pass
    {
    public:
        BRDF_LUT_Pass(unsigned int width, unsigned int height);
        ~BRDF_LUT_Pass();

        void render(Camera* camera, Scene* scene) override;
        const std::vector<shared_attachment> get_attachments() override;
        void resize(unsigned int width, unsigned int height) override;

        shared_attachment m_attach_lut;

    private:
    };
}