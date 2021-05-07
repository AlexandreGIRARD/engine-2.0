#pragma once

#include "pipeline/render_pass.hpp"

namespace pipeline
{
    class Screen_Pass : public Render_Pass
    {
    public:
        Screen_Pass(unsigned width, unsigned int height);


        virtual void render(Camera* camera, Scene* scene) = 0;
        virtual const std::vector<shared_attachment> get_attachments() = 0;

    protected:
        void set_screen_quad();
        void render_screen_quad();

        unsigned int m_quad_vao;
        unsigned int m_quad_vbo;
    };
}