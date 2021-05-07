#pragma once

#include "screen_pass.hpp"

namespace pipeline
{
    class Debug_Pass : public Screen_Pass
    {
    public:
        Debug_Pass(unsigned int width, unsigned int height);
        ~Debug_Pass();

        void render(Camera* camera, Scene* scene) override;
        const std::vector<shared_attachment> get_attachments() override;
        void resize(unsigned int width, unsigned int height) override;
    
        void set_attachment(const shared_attachment attachment, const int mode);

        shared_attachment m_attach_output;

    private:
        int m_mode;
    };
}