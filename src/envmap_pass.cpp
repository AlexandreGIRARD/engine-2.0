#include "envmap_pass.hpp"

#include "texture.hpp"

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

EnvMap_Pass::EnvMap_Pass()
    : Render_Pass(false)
{
    // Init skybox program
    m_program->add_shader("envmap/cubemap.vert", GL_VERTEX_SHADER);
    m_program->add_shader("envmap/cubemap.geom", GL_GEOMETRY_SHADER);
    m_program->add_shader("envmap/envmap.frag", GL_FRAGMENT_SHADER);
    m_program->link();

    // Init irradiance program
    m_irradiance_program = std::make_shared<Program>();
    m_irradiance_program->add_shader("envmap/cubemap.vert", GL_VERTEX_SHADER);
    m_irradiance_program->add_shader("envmap/cubemap.geom", GL_GEOMETRY_SHADER);
    m_irradiance_program->add_shader("envmap/irradiance.frag", GL_FRAGMENT_SHADER);
    m_irradiance_program->link();

    // Init specular program
    m_specular_program = std::make_shared<Program>();
    m_specular_program->add_shader("envmap/cubemap.vert", GL_VERTEX_SHADER);
    m_specular_program->add_shader("envmap/cubemap.geom", GL_GEOMETRY_SHADER);
    m_specular_program->add_shader("envmap/specular.frag", GL_FRAGMENT_SHADER);
    m_specular_program->link();

    // Init attachments
    m_attach_skybox     = std::make_shared<Attachment>(GL_TEXTURE_CUBE_MAP, 512, 512, GL_RGB, GL_FLOAT);
    m_attach_irradiance = std::make_shared<Attachment>(GL_TEXTURE_CUBE_MAP, 64, 64, GL_RGB, GL_FLOAT);
    m_attach_irradiance = std::make_shared<Attachment>(GL_TEXTURE_CUBE_MAP, 256, 256, GL_RGB, GL_FLOAT);

    // Init framebuffer
    m_fbo->bind();

    // Init Screen-Quad
    set_cube();
    set_views();
}

EnvMap_Pass::~EnvMap_Pass()
{
    delete m_hdr_tex;
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
    // Do nothing this pass is useless
}

const std::vector<shared_attachment> EnvMap_Pass::get_attachments()
{
    return std::vector<shared_attachment>{m_attach_skybox, m_attach_irradiance, m_attach_specular};
}

void EnvMap_Pass::init_env_maps(const std::string& file_path, const int id)
{
    if (m_hdr_tex)
        delete m_hdr_tex;

    // Load new HDRI map
    m_hdr_tex = new Texture(("resources/hdri/" + file_path+ ".hdr"), true);
    m_hdr_id = id;

    render_skybox();
    // render_irradiance();
    // render_specular();
}

void EnvMap_Pass::bind_matrices(shared_program program)
{
    program->addUniformMat4(m_projection, "projection");
    for (int i = 0; i < m_views.size(); i++)
        program->addUniformMat4(m_views[i], ("views[" + std::to_string(i) + "]").c_str());
}

void EnvMap_Pass::render_cubemap(shared_program program, shared_attachment attachment)
{
    bind_matrices(program);

    m_fbo->bind();
    m_fbo->set_attachment(attachment, GL_COLOR_ATTACHMENT0);
    m_fbo->unbind();

    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo->get_name());
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
    glViewport(0, 0, 512, 512);
    render_cubemap(m_program, m_attach_skybox);
}

void EnvMap_Pass::render_irradiance()
{
    m_irradiance_program->use();

    // Skybox Cubemap binding
    m_irradiance_program->addUniformTexture(0, "skybox");
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_attach_skybox->m_name);

    glViewport(0, 0, 64, 64);
    render_cubemap(m_irradiance_program, m_attach_irradiance);
}

void EnvMap_Pass::render_specular()
{
    m_specular_program->use();

    // Skybox Cubemap binding
    m_specular_program->addUniformTexture(0, "skybox");
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_attach_skybox->m_name);

    glViewport(0, 0, 256, 256);
    render_cubemap(m_specular_program, m_attach_specular);
}