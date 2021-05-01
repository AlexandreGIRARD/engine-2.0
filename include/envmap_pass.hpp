#pragma once

#include "render_pass.hpp"

class Texture;

class EnvMap_Pass : Render_Pass
{
public:
    EnvMap_Pass();
    ~EnvMap_Pass();

    void render(Camera* camera, Scene* scene) override;
    const std::vector<shared_attachment> get_attachments() override;

    void init_env_maps(const std::string& file_path, const int id);

    shared_attachment m_attach_skybox;
    shared_attachment m_attach_irradiance;
    shared_attachment m_attach_specular;

private:
    void set_cube();
    void set_views();
    void bind_matrices(shared_program program);

    void render_cubemap(shared_program program, shared_attachment attachment);
    void render_skybox();
    void render_irradiance();
    void render_specular();

    // HDRI texture
    int      m_hdr_id;
    Texture* m_hdr_tex;

    // Cube info
    unsigned int m_cube_vao;
    unsigned int m_cube_vbo;
    glm::mat4  m_projection;
    std::vector<glm::mat4> m_views;

    // Other program
    shared_program m_irradiance_program;
    shared_program m_specular_program;
};