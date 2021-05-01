#pragma once

// OpenGL includes
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// Imgui includes
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include <memory>
#include <string>

class Scene;
class Camera;
class G_Buffer_Pass;
class Deferred_Pass;
class EnvMap_Pass;

using shared_scene  = std::shared_ptr<Scene>;
using shared_camera = std::shared_ptr<Camera>;

struct Renderer_Info
{
    // Environment Map Infos
    int current_hdr_map = 0;
    const char* hdr_files[2] = {"grass_field", "studio"};
    //    
};

class Renderer
{
public:
    Renderer(int major, int minor, const char* name);
    ~Renderer();

    void loop();
private:
    bool init_window(int major, int minor);
    bool init_imgui();
    bool init_pipeline();
    void update_imgui();
    void render_imgui();
    void render(double xpos, double ypos);

    Scene*  m_scene;
    Camera* m_camera;

    GLFWwindow* m_window;

    double m_time;
    double m_delta;

    unsigned int m_width;
    unsigned int m_height;

    // Pass
    G_Buffer_Pass* m_gbuffer_pass;
    Deferred_Pass* m_deferred_pass;
    EnvMap_Pass*   m_envmap_pass;

    Renderer_Info m_infos;
};