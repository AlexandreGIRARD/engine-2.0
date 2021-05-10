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

namespace pipeline
{
    class G_Buffer_Pass;
    class Deferred_Pass;
    class EnvMap_Pass;
    class Skybox_Pass;
    class BRDF_LUT_Pass;
    class AO_Pass;
    class AA_Pass;
    class Debug_Pass;
}

using shared_scene  = std::shared_ptr<Scene>;
using shared_camera = std::shared_ptr<Camera>;

enum Debug_Mode
{
    NONE      = -1,
    G_POSITION  = 0,
    G_COLOR,
    G_NORMAL,
    G_METALLIC,
    G_ROUGHNESS,
    G_EMISSIVE,
    G_AO,
    AMBIENT_OCCLUSION,
};

struct Renderer_Info
{
    // Environment Map Infos
    int current_hdr_map = 0;
    const char* hdr_files[2] = {"studio", "grass_field"};
    
    // Debug Texture mode Infos
    bool debug = false;
    Debug_Mode mode = Debug_Mode::G_POSITION; 
    const char* modes[8] = {"Position", "Base Color", "Normal", "Metallic", "Roughness", "Emissive", "Ambient-Occlusion", "SSAO"};
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
    int render_debug();
    void resize(unsigned int width, unsigned int height);

    Scene*  m_scene;
    Camera* m_camera;

    GLFWwindow* m_window;

    double m_time;
    double m_delta;

    unsigned int m_width;
    unsigned int m_height;

    // Pass
    pipeline::G_Buffer_Pass* m_gbuffer_pass;
    pipeline::Deferred_Pass* m_deferred_pass;
    pipeline::EnvMap_Pass*   m_envmap_pass;
    pipeline::Skybox_Pass*   m_skybox_pass;
    pipeline::BRDF_LUT_Pass* m_brdf_lut_pass;
    pipeline::AO_Pass*       m_ao_pass;
    pipeline::AA_Pass*       m_aa_pass;
    pipeline::Debug_Pass*    m_debug_pass;

    Renderer_Info m_infos;
};