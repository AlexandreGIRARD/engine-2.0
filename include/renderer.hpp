#pragma once

// OpenGL includes
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// Imgui includes
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include "scene.hpp"
#include "camera.hpp"

class Renderer
{
public:
    Renderer(int major, int minor, const char* name);
    ~Renderer();

    void loop();
private:
    bool init_window(int major, int minor);
    bool init_imgui();
    void render_imgui();
    void render(double xpos, double ypos);

    Scene  m_scene;
    Camera m_camera;

    GLFWwindow* m_window;

    double m_time;
    double m_delta;
};