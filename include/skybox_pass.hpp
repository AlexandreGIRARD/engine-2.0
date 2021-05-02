#pragma once

#include "render_pass.hpp"

class Skybox_Pass : public Render_Pass
{
public:
    Skybox_Pass(unsigned int width, unsigned int height);
    ~Skybox_Pass();

    void render(Camera* camera, Scene* scene) override;
    const std::vector<shared_attachment> get_attachments() override;
    void resize(unsigned int width, unsigned int height) override;

    void blit_buffers(const shared_fbo color_fbo, const shared_fbo depth_fbo);
    void set_skybox_attachments(const std::vector<shared_attachment> attachments);

    shared_attachment m_attach_color; // Color attachment for drawing skybox
    shared_attachment m_attach_depth; // Depth attachment for drawing skybox

private:
    void set_cube();

    // Cube info
    unsigned int m_cube_vao;
    unsigned int m_cube_vbo;
};