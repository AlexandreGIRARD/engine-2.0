#pragma once

#include "render_pass.hpp"

class Light;
using shared_light = std::shared_ptr<Light>;

class Deferred_Pass : public Render_Pass
{
public:
    Deferred_Pass(unsigned int width, unsigned int height);
    ~Deferred_Pass();

    void render(Camera* camera, Scene* scene) override;
    const std::vector<shared_attachment> get_attachments() override;
    void resize(unsigned int width, unsigned int height) override;

    void set_lights(std::vector<shared_light> lights);
    void set_gbuffer_attachments(const std::vector<shared_attachment> g_buffer_attchments);
    void set_ssao_attachment(const shared_attachment occlusion_attachment);
    void set_ibl_attachments(const std::vector<shared_attachment> ibl_attchments);

    shared_attachment m_attach_output;

private:
    void set_screen_quad();
    void render_screen_quad();

    unsigned int m_quad_vao;
    unsigned int m_quad_vbo;
};