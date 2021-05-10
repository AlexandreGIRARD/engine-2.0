#pragma once

#include "pipeline/screen_pass.hpp"

namespace pipeline
{
    // http://developer.download.nvidia.com/assets/gamedev/files/sdk/11/FXAA_WhitePaper.pdf
    class AA_Pass : public Screen_Pass
    {
    public:
        AA_Pass(unsigned int width, unsigned int height);
        ~AA_Pass();

        void render(Camera* camera, Scene* scene) override;
        const std::vector<shared_attachment> get_attachments() override;
        void resize(unsigned int width, unsigned int height) override;

        void set_frame_attachments(const std::vector<shared_attachment> g_frame_attchments);

        shared_attachment m_attach_output;

        bool* get_debug() { return &m_is_debug; }

    private:
        bool m_is_debug = false;
    };
}