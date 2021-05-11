#include "renderer.hpp"

#include <iostream>

#include "scene.hpp"
#include "camera.hpp"
#include "utils.hpp"

#include "pipeline/all_passes.hpp"

static bool camera_reset_pos;
static bool move;

static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS)
    {
        camera_reset_pos = true;
        move = true;
    }
    if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_RELEASE)
    {
        camera_reset_pos = false;
        move = false;
    }
}

Renderer::Renderer(int major, int minor, const char* path)
{
    if (!init_window(major, minor))
        exit(1);
    init_imgui();
    init_pipeline();
    m_scene = new Scene(path);
    m_camera = new Camera();
}

Renderer::~Renderer()
{
    delete m_scene;
    delete m_camera;

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
}

bool Renderer::init_window(int major, int minor)
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_width = 1920;
    m_height = 1080;
    m_window = utils::init_window(m_width, m_height);
    glfwMakeContextCurrent(m_window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD\n";
        return false;
    }

    // Debug
#ifdef DEBUG
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(utils::debug_callback, 0);
#endif

    // Tell OpenGL window size
    glViewport(0, 0, m_width, m_height);
    glfwSetFramebufferSizeCallback(m_window, utils::framebuffer_size_callback);

    // Enable z-buffer computation
    glEnable(GL_DEPTH_TEST);

    // Mouse event setup
    glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    glfwSetMouseButtonCallback(m_window, mouse_button_callback); //<-- setup func in utils.hpp

    return m_window != nullptr;
}

bool Renderer::init_imgui()
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(m_window, true);
    ImGui_ImplOpenGL3_Init();
    return true;
}

bool Renderer::init_pipeline()
{
    m_gbuffer_pass  = new pipeline::G_Buffer_Pass(m_width, m_height);
    m_deferred_pass = new pipeline::Deferred_Pass(m_width, m_height);
    m_envmap_pass   = new pipeline::EnvMap_Pass(m_width, m_height);
    m_skybox_pass   = new pipeline::Skybox_Pass(m_width, m_height);
    m_brdf_lut_pass = new pipeline::BRDF_LUT_Pass(m_width, m_height);
    m_ao_pass       = new pipeline::AO_Pass(m_width, m_height);
    m_aa_pass       = new pipeline::AA_Pass(m_width, m_height);
    m_debug_pass    = new pipeline::Debug_Pass(m_width, m_height);
    m_bloom_pass    = new pipeline::Bloom_Pass(m_width, m_height);
    return true;
}

void Renderer::loop()
{
    double xpos, ypos;
    bool move_cursor = true;

    // Pre-Loop initialisation
    m_brdf_lut_pass->render(nullptr, nullptr);

    // Render loop
    while(!glfwWindowShouldClose(m_window))
    {
        // Clear bufffer
        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Callback quit window with KEY_ESCAPE
        utils::quit_window(m_window);

        // Check if window was resized
        int width, height;
        glfwGetFramebufferSize(m_window, &width, &height);
        if (width != m_width || height != m_height)
            resize(width, height);

        // Compute delta time
        m_delta = glfwGetTime() - m_time;
        m_time = glfwGetTime();
        utils::frame_rate(m_time);

        // Get mouse event (position variations)
        glfwGetCursorPos(m_window, &xpos, &ypos);

        // UI update
        update_imgui();

        render(xpos, ypos);

        // Render dear imgui into screen
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Check and call events
        glfwSwapBuffers(m_window);
        glfwPollEvents();
    }
}

void Renderer::update_imgui()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("PBR-Engine");
    
    // Camera settings
    ImGui::Text("Camera Settings:");
    ImGui::SliderFloat("Fov", m_camera->get_fov(), 30.f, 120.f);
    ImGui::SliderFloat("Near Plane", m_camera->get_near(), 0.0001f, 0.1f);
    ImGui::SliderFloat("Far Plane", m_camera->get_far(), 20.f, 10000.f);
    ImGui::Separator();

    // Environment settings
    ImGui::Text("Environment Settings:");
    ImGui::Combo("Current Environment", &m_infos.current_hdr_map, m_infos.hdr_files, sizeof(m_infos.hdr_files) / sizeof(char*));
    ImGui::SliderFloat("IBL Factor", m_deferred_pass->get_ibl_factor(), 0.f, 1.f);
    ImGui::Separator();

    // Ambient Occlusion settings
    ImGui::Text("Ambient Occlusion Settings");
    ImGui::SliderFloat("Radius", m_ao_pass->get_radius(), 0.001f, 1.0f);
    ImGui::SliderFloat("Power", m_ao_pass->get_power(), 0.5f, 10.f);
    ImGui::Separator();

    // Debug Texture Mode
    ImGui::Checkbox("Debug Texture Mode", &m_infos.debug);
    if (m_infos.debug)
        ImGui::Combo("Mode", (int*)&m_infos.mode, m_infos.modes, sizeof(m_infos.modes) / sizeof(char*));
    
    // Anti Aliasing 
    ImGui::Checkbox("Anti-Aliasing (FXAA)", &m_infos.aa_activated);
    if (m_infos.aa_activated)
        ImGui::Checkbox("Debug", m_aa_pass->get_debug());
    
    // Bloom
    ImGui::Checkbox("Bloom", &m_infos.bloom_activated);
    if (m_infos.bloom_activated)
    {
        ImGui::SliderFloat("Threshold", m_bloom_pass->get_threshold(), 0.f, 1.f);
        ImGui::SliderFloat("Strength", m_bloom_pass->get_strength(), 0.5f, 2.f);
    }

    if (ImGui::TreeNode("FX"))
    {
        ImGui::TreePop();
    }
    ImGui::ShowDemoWindow();
    ImGui::End();
}

void Renderer::render(double xpos, double ypos)
{
    if (camera_reset_pos) {
        m_camera->reset_mouse_pos();
        camera_reset_pos = false;
    }
    m_camera->update(m_window, m_delta, xpos, ypos, move);

    // Update skybox
    int hdr_map_id = m_infos.current_hdr_map; 
    if (hdr_map_id != m_envmap_pass->get_current_hdr_map()) 
    {
        m_envmap_pass->init_env_maps(m_infos.hdr_files[hdr_map_id], hdr_map_id);
        m_skybox_pass->set_skybox_attachments(m_envmap_pass->get_attachments());
        m_deferred_pass->set_ibl_attachments(m_envmap_pass->get_attachments());
        m_deferred_pass->set_brdf_lut_attachment(m_brdf_lut_pass->get_attachments());
    }

    // G-Buffer Pass    
    m_gbuffer_pass->render(m_camera, m_scene);

    // Occlusion Pass
    m_ao_pass->set_gbuffer_attachments(m_gbuffer_pass->get_attachments());
    m_ao_pass->render(m_camera, m_scene);

    // CSM Pass

    // Final Deferred Pass
    m_deferred_pass->set_lights(m_scene->get_lights());
    m_deferred_pass->set_gbuffer_attachments(m_gbuffer_pass->get_attachments());
    m_deferred_pass->set_ssao_attachment(m_ao_pass->get_attachments());
    m_deferred_pass->render(m_camera, m_scene);

    // Render Skybox pass
    m_skybox_pass->blit_buffers(m_deferred_pass->get_fbo(), m_gbuffer_pass->get_fbo());
    m_skybox_pass->render(m_camera, m_scene);

    // FBO name to blit
    auto attachments = m_skybox_pass->get_attachments();
    int fbo_name = m_skybox_pass->get_fbo()->get_name();
    if (m_infos.debug)
    {
        fbo_name = render_debug();
        attachments = m_debug_pass->get_attachments();
    }

    // Anti-Aliasing
    if (m_infos.aa_activated)
    {
        m_aa_pass->set_frame_attachments(attachments);
        m_aa_pass->render(nullptr, nullptr);
        fbo_name = m_aa_pass->get_fbo()->get_name();
        attachments = m_aa_pass->get_attachments();
    }

    // Bloom
    if (m_infos.bloom_activated)
    {
        m_bloom_pass->set_frame_attachments(attachments);
        m_bloom_pass->render(nullptr, nullptr);
        //fbo_name = m_bloom_pass->get_fbo()->get_name();
        //attachments = m_bloom_pass->get_attachments();
    }

    // FX 

    // Blit
    glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo_name);
    glBlitFramebuffer(0, 0, m_width, m_height, 0, 0, m_width, m_height, GL_COLOR_BUFFER_BIT, GL_LINEAR);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::resize(unsigned int width, unsigned int height)
{
    m_gbuffer_pass->resize(width, height);
    m_deferred_pass->resize(width, height);
    m_envmap_pass->resize(width, height);
    m_skybox_pass->resize(width, height);
    m_ao_pass->resize(width, height);
    m_aa_pass->resize(width, height);
    m_debug_pass->resize(width, height);
    m_bloom_pass->resize(width, height);
}

int Renderer::render_debug()
{
    shared_attachment attach;

    if (m_infos.mode != Debug_Mode::AMBIENT_OCCLUSION)
    {
        auto attachments = m_gbuffer_pass->get_attachments();
        switch (m_infos.mode)
        {
        case Debug_Mode::G_POSITION:
            attach = attachments[0];
            break;
        case Debug_Mode::G_COLOR:
            attach = attachments[1];
            break;
        case Debug_Mode::G_EMISSIVE:
            attach = attachments[3];
            break;
        case Debug_Mode::G_NORMAL:
            attach = attachments[4];
            break;
        default:
            attach = attachments[2];
            break;
        }
    }
    else
    {
        attach = m_ao_pass->get_attachments()[1]; // We take second one, blur
    }

    m_debug_pass->set_attachment(attach, static_cast<int>(m_infos.mode));
    m_debug_pass->render(nullptr, nullptr);

    return m_debug_pass->get_fbo()->get_name();
}