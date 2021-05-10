#include "utils.hpp"

#include <stb/stb_image.h>

namespace utils
{
    static float switch_off = 0;
    static float fps = 0.f;
    static float last_time = 0.f;

    float frame_rate(float time)
    {
        float current_time = time;
        ++fps;
        if( current_time - last_time > 1.0f )
        {
            last_time = current_time;
            fps = 0;
        }
        return fps;
    }
    void GLAPIENTRY debug_callback(GLenum source,
                                GLenum type,
                                GLuint id,
                                GLenum severity,
                                GLsizei length,
                                const GLchar* message,
                                const void* userParam )
    {
        {
            const char* _source;
            const char* _type;
            const char* _severity;

            switch (source) {
                case GL_DEBUG_SOURCE_API:
                _source = "API";
                break;

                case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
                _source = "WINDOW SYSTEM";
                break;

                case GL_DEBUG_SOURCE_SHADER_COMPILER:
                _source = "SHADER COMPILER";
                break;

                case GL_DEBUG_SOURCE_THIRD_PARTY:
                _source = "THIRD PARTY";
                break;

                case GL_DEBUG_SOURCE_APPLICATION:
                _source = "APPLICATION";
                break;

                case GL_DEBUG_SOURCE_OTHER:
                _source = "UNKNOWN";
                break;

                default:
                _source = "UNKNOWN";
                break;
            }

            switch (type) {
                case GL_DEBUG_TYPE_ERROR:
                _type = "ERROR";
                break;

                case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
                _type = "DEPRECATED BEHAVIOR";
                break;

                case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
                _type = "UDEFINED BEHAVIOR";
                break;

                case GL_DEBUG_TYPE_PORTABILITY:
                _type = "PORTABILITY";
                break;

                case GL_DEBUG_TYPE_PERFORMANCE:
                _type = "PERFORMANCE";
                break;

                case GL_DEBUG_TYPE_OTHER:
                _type = "OTHER";
                break;

                case GL_DEBUG_TYPE_MARKER:
                _type = "MARKER";
                break;

                default:
                _type = "UNKNOWN";
                break;
            }

            switch (severity) {
                case GL_DEBUG_SEVERITY_HIGH:
                _severity = "HIGH";
                break;

                case GL_DEBUG_SEVERITY_MEDIUM:
                _severity = "MEDIUM";
                break;

                case GL_DEBUG_SEVERITY_LOW:
                _severity = "LOW";
                break;

                case GL_DEBUG_SEVERITY_NOTIFICATION:
                _severity = "NOTIFICATION";
                break;

                default:
                _severity = "UNKNOWN";
                break;
            }

            fprintf(stderr, "GL ERROR: Type = %s, Severity = %s, Message = %s\n",
                _type, _severity, message );
        }
    }

    void quit_window(GLFWwindow *window)
    {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);
    }

    void move_window(GLFWwindow *window, double *xpos, double *ypos)
    {
        if (glfwGetKey(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS)
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            glfwGetCursorPos(window, xpos, ypos);
        }
        if (glfwGetKey(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_RELEASE)
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
    }

    float pause_rotation(GLFWwindow *window, float rad_off)
    {
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        {
            auto tmp = switch_off;
            switch_off = rad_off;
            return tmp;
        }
        return rad_off;
    }

    void framebuffer_size_callback(GLFWwindow *window, int width, int height)
    {
        glViewport(0, 0, width, height);
    }


    GLFWwindow *init_window(unsigned int width, unsigned int height)
    {
        GLFWwindow *window = glfwCreateWindow(width, height, "PBR-Engine v0.2", NULL, NULL);
        if (!window)
        {
            std::cout << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
            exit(-1);
        }
        return window;
    }
}