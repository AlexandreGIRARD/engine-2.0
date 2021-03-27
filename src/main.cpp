#include <iostream>
#include <cmath>
#include <filesystem>
#include <memory>

#include "renderer.hpp"
#include "utils.hpp"


static double xpos, ypos;
static bool move_cursor;
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS)
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        move_cursor=true;
    }
    if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_RELEASE)
    {
        move_cursor=false;
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
}


int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cerr << "Usage: ./engine <scene.glb>" << std::endl;
        exit(1);
    }

    Renderer renderer = Renderer(4, 5, argv[1]);
    renderer.loop();

    return 0;
}
