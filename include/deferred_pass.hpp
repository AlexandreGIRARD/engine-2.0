#pragma once

#include "render_pass.hpp"

class Deferred_Pass : public Render_Pass
{
public:
    Deferred_Pass(unsigned int width, unsigned int height);
    ~Deferred_Pass();

    void render(Camera* camera, Scene* scene) override;
    void set_gbuffer_attachments(const shared_attachment g_buffer_attchments[5]);
    void set_ssao_attachments(const shared_attachment occlusion_attchment);

    shared_attachment m_attach_output;

private:
    void set_screen_quad();
    void render_screen_quad();

    unsigned int m_quad_vao;
    unsigned int m_quad_vbo;
};