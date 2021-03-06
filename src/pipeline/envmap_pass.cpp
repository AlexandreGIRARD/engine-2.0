#include "pipeline/envmap_pass.hpp"

#include "texture.hpp"

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

using namespace pipeline;

EnvMap_Pass::EnvMap_Pass(unsigned int width, unsigned int height)
    : Render_Pass(width, height, false)
{
    // Init Skybox Generation program
    m_program->add_shader("envmap/cubemap.vert", GL_VERTEX_SHADER);
    m_program->add_shader("envmap/cubemap.geom", GL_GEOMETRY_SHADER);
    m_program->add_shader("envmap/envmap.frag", GL_FRAGMENT_SHADER);
    m_program->link();

    // Init Irradiance Generation program
    m_irradiance_program = std::make_shared<Program>();
    m_irradiance_program->add_shader("envmap/cubemap.vert", GL_VERTEX_SHADER);
    m_irradiance_program->add_shader("envmap/cubemap.geom", GL_GEOMETRY_SHADER);
    m_irradiance_program->add_shader("envmap/irradiance.frag", GL_FRAGMENT_SHADER);
    m_irradiance_program->link();

    // Init Specular Generation program
    m_specular_program = std::make_shared<Program>();
    m_specular_program->add_shader("envmap/cubemap.vert", GL_VERTEX_SHADER);
    m_specular_program->add_shader("envmap/cubemap.geom", GL_GEOMETRY_SHADER);
    m_specular_program->add_shader("envmap/specular.frag", GL_FRAGMENT_SHADER);
    m_specular_program->link();

    // Init attachments
    m_attach_skybox_map         = std::make_shared<Attachment>(GL_TEXTURE_CUBE_MAP, 1024, 1024, GL_RGB32F, GL_RGB, GL_FLOAT);
    m_attach_irradiance_cubemap = std::make_shared<Attachment>(GL_TEXTURE_CUBE_MAP, 128, 128, GL_RGB32F, GL_RGB, GL_FLOAT);
    m_attach_specular_cubemap   = std::make_shared<Attachment>(GL_TEXTURE_CUBE_MAP, 512, 512, GL_RGB32F, GL_RGB, GL_FLOAT);
    m_attach_specular_cubemap->generate_mipmap();

    // Init Cube
    set_cube();
    set_views();
}

EnvMap_Pass::~EnvMap_Pass()
{
    delete m_hdr_tex;
}

void EnvMap_Pass::resize(unsigned int width, unsigned int height)
{
    Render_Pass::resize(width, height);
}

void EnvMap_Pass::set_cube()
{
    const float skybox_cube[] = {
        // positions
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    glGenVertexArrays(1, &m_cube_vao);
    glGenBuffers(1, &m_cube_vbo);

    glBindVertexArray(m_cube_vao);

    glBindBuffer(GL_ARRAY_BUFFER, m_cube_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skybox_cube), &skybox_cube, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
}

void EnvMap_Pass::set_views()
{
    m_projection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
    m_views.emplace_back(glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)));
    m_views.emplace_back(glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)));
    m_views.emplace_back(glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)));
    m_views.emplace_back(glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)));
    m_views.emplace_back(glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)));
    m_views.emplace_back(glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f)));
}

void EnvMap_Pass::render(Camera* camera, Scene* scene)
{
    // Do nothing
}

const std::vector<shared_attachment> EnvMap_Pass::get_attachments()
{
    return std::vector<shared_attachment>{m_attach_skybox_map, m_attach_irradiance_cubemap, m_attach_specular_cubemap};
}

void EnvMap_Pass::init_env_maps(const std::string& file_path, const int id)
{
    if (m_hdr_tex)
        delete m_hdr_tex;

    // Load new HDRI map
    m_hdr_tex = new Texture(("resources/hdri/" + file_path+ ".hdr"), true);
    m_hdr_id = id;

    render_skybox();
    render_irradiance();
    render_specular();
}

void EnvMap_Pass::bind_matrices(shared_program program)
{
    program->addUniformMat4(m_projection, "projection");
    for (int i = 0; i < m_views.size(); i++)
        program->addUniformMat4(m_views[i], ("views[" + std::to_string(i) + "]").c_str());
}

void EnvMap_Pass::render_cubemap(shared_program program, shared_attachment attachment, int level)
{
    bind_matrices(program);

    m_fbo->bind();
    m_fbo->set_attachment(attachment, GL_COLOR_ATTACHMENT0, level);

    glClear(GL_COLOR_BUFFER_BIT);
    glDrawBuffer(GL_COLOR_ATTACHMENT0);

    glBindVertexArray(m_cube_vao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 36);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void EnvMap_Pass::render_skybox()
{
    m_program->use();
    m_hdr_tex->bind(m_program, 0, "hdr_map");
    glViewport(0, 0, m_attach_skybox_map->m_width, m_attach_skybox_map->m_height);
    render_cubemap(m_program, m_attach_skybox_map);
    glViewport(0, 0, m_width, m_height);
}

void EnvMap_Pass::render_irradiance()
{
    m_irradiance_program->use();
    m_irradiance_program->addUniformTextureCubeMap(m_attach_skybox_map->m_name, 0, "skybox"); // Skybox Cubemap binding

    glViewport(0, 0, m_attach_irradiance_cubemap->m_width, m_attach_irradiance_cubemap->m_height);
    render_cubemap(m_irradiance_program, m_attach_irradiance_cubemap);
    glViewport(0, 0, m_width, m_height);
}

void EnvMap_Pass::render_specular()
{
    m_specular_program->use();
    m_specular_program->addUniformTextureCubeMap(m_attach_skybox_map->m_name, 0, "skybox"); //  Skybox Cubemap binding
    m_attach_skybox_map->generate_mipmap();
    
    int max_level = 5;
    for (int level = 0; level < max_level; level++)
    {
        int size = m_attach_specular_cubemap->m_width * std::pow(0.5, level);
        glViewport(0, 0, size, size);

        float roughness = static_cast<float>(level) / static_cast<float>(max_level - 1);
        m_specular_program->addUniformFloat(roughness, "roughness");

        render_cubemap(m_specular_program, m_attach_specular_cubemap, level);
    }
    glViewport(0, 0, m_width, m_height);
}