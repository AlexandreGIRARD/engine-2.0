#pragma once

// OpenGL includes
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// Imgui includes
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include <memory>

class Scene;
class Camera;
class Render_Pass;

using shared_scene  = std::shared_ptr<Scene>;
using shared_camera = std::shared_ptr<Camera>;
using shared_pass   = std::shared_ptr<Render_Pass>;

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
    void render_imgui();
    void render(double xpos, double ypos);

    shared_scene  m_scene;
    shared_camera m_camera;

    GLFWwindow* m_window;

    double m_time;
    double m_delta;

    // Pass
    shared_pass m_basic_pass;
};