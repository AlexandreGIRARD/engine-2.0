#include "renderer.hpp"

#include <iostream>

#include "scene.hpp"
#include "camera.hpp"
#include "render_pass.hpp"
#include "utils.hpp"
#include "g_buffer_pass.hpp"
#include "deferred_pass.hpp"
#include "envmap_pass.hpp"
#include "skybox_pass.hpp"

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
    m_gbuffer_pass  = new G_Buffer_Pass(m_width, m_height);
    m_deferred_pass = new Deferred_Pass(m_width, m_height);
    m_envmap_pass   = new EnvMap_Pass(m_width, m_height);
    m_skybox_pass   = new Skybox_Pass(m_width, m_height);
    return true;
}

void Renderer::loop()
{
    double xpos, ypos;
    bool move_cursor = true;

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
    // Change camera settings
    if (ImGui::TreeNode("Camera"))
    {
        ImGui::SliderFloat("Fov", m_camera->get_fov(), 30.f, 120.f);
        ImGui::SliderFloat("Near Plane", m_camera->get_near(), 0.0001f, 0.1f);
        ImGui::SliderFloat("Far Plane", m_camera->get_far(), 20.f, 10000.f);
        ImGui::TreePop();
    }
    ImGui::Combo("Current Environment", &m_infos.current_hdr_map, m_infos.hdr_files, sizeof(m_infos.hdr_files) / sizeof(char*));

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
    }

    // G-Buffer Pass    
    m_gbuffer_pass->render(m_camera, m_scene);

    // Occlusion Pass

    // CSM Pass

    // Final Deferred Pass
    m_deferred_pass->set_lights(m_scene->get_lights());
    m_deferred_pass->set_gbuffer_attachments(m_gbuffer_pass->get_attachments());
    m_deferred_pass->render(m_camera, m_scene);

    // Render Skybox pass
    m_skybox_pass->blit_buffers(m_deferred_pass->get_fbo(), m_gbuffer_pass->get_fbo());
    m_skybox_pass->render(m_camera, m_scene);

    glBindFramebuffer(GL_READ_FRAMEBUFFER, m_skybox_pass->get_fbo()->get_name());
    glBlitFramebuffer(0, 0, m_width, m_height, 0, 0, m_width, m_height, GL_COLOR_BUFFER_BIT, GL_LINEAR);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::resize(unsigned int width, unsigned int height)
{
    m_gbuffer_pass->resize(width, height);
    m_deferred_pass->resize(width, height);
    m_envmap_pass->resize(width, height);
    m_skybox_pass->resize(width, height);
}