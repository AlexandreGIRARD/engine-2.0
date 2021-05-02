#pragma once

#include "render_pass.hpp"

class Texture;

class EnvMap_Pass : public Render_Pass
{
public:
    EnvMap_Pass(unsigned int width, unsigned int height);
    ~EnvMap_Pass();

    void render(Camera* camera, Scene* scene) override;
    const std::vector<shared_attachment> get_attachments() override;
    void resize(unsigned int width, unsigned int height) override;

    void init_env_maps(const std::string& file_path, const int id);
    const int get_current_hdr_map() { return m_hdr_id; }

    shared_attachment m_attach_skybox_map; // Generated Skybox CUBEMAP 
    shared_attachment m_attach_irradiance_cubemap; // Generated Irradiance CUBEMAP
    shared_attachment m_attach_specular_cubemap; // Generated Specular CUBEMAP

private:
    void set_cube();
    void set_views();
    void bind_matrices(shared_program program);

    void render_cubemap(shared_program program, shared_attachment attachment);
    void render_skybox();
    void render_irradiance();
    void render_specular();

    // HDRI texture
    int      m_hdr_id  = -1;
    Texture* m_hdr_tex = nullptr;

    // Cube info
    unsigned int m_cube_vao;
    unsigned int m_cube_vbo;
    glm::mat4  m_projection;
    std::vector<glm::mat4> m_views;

    // Other program
    shared_program m_irradiance_program;
    shared_program m_specular_program;
};