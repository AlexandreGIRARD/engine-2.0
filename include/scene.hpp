#pragma once

// OpenGL includes
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// Imgui includes
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

// Personal renderer classes
#include "node.hpp"
#include "camera.hpp"
#include "mesh.hpp"
#include "material.hpp"
#include "light.hpp"

using shared_node = std::shared_ptr<Node>;
using shared_material = std::shared_ptr<Material>;
using shared_light = std::shared_ptr<Light>;
using shared_camera = std::shared_ptr<Camera>;
using shared_mesh = std::shared_ptr<Mesh>;

class Scene
{
public:
    Scene(const char* path);

    void render(GLFWwindow* window, double delta, size_t xpos, size_t ypos);

private:
    bool create_scene(tinygltf::Model& model);

    shared_camera                m_camera;

    std::vector<shared_material> m_materials;
    std::vector<shared_mesh>     m_meshes;
    std::vector<shared_node>     m_nodes;
    std::vector<shared_light>    m_lights;
};