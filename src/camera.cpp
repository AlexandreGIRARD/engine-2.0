#include "camera.hpp"

#include "GLFW/glfw3.h"

#include "glm/gtc/matrix_transform.hpp"

// default one
Camera::Camera()
    : m_position(0,0,-5)
    , m_target(0,0,0)
    , m_up(0,1,0)
    , m_forward(0,0,1)
    , m_right(- glm::cross(m_up, m_forward))
    , m_speed(5)
    , m_fov(45)
    , m_yaw(90.f)
    , m_pitch(0.f)
    , m_near(0.1)
    , m_far(20)
{
    m_projection = glm::perspective(glm::radians(m_fov), 1920/1080.f, m_near, m_far);
}

Camera::Camera(glm::vec3 position, glm::vec3 target, glm::vec3 up, float fov, float speed, float near, float far, float ratio)
    : m_position(position)
    , m_target(target)
    , m_up(up)
    , m_forward(target - position)
    , m_right(- glm::cross(up, m_forward))
    , m_speed(speed)
    , m_fov(fov)
    , m_yaw(90.f)
    , m_pitch(0.f)
    , m_near(near)
    , m_far(far)
{
    m_projection = glm::perspective(glm::radians(m_fov), ratio, m_near, m_far);
}

Camera::Camera(glm::vec3 position, glm::vec3 target, glm::vec3 up)
    : m_position(position)
    , m_target(target)
    , m_up(up)
    , m_forward(target - position)
    , m_right(- glm::cross(up, m_forward))
    , m_speed(0)
    , m_fov(90.f)
    , m_yaw(90.f)
    , m_pitch(0.f)
    , m_near(1)
    , m_far(10)
{}

Camera::Camera(const Camera &camera)
    : m_position(glm::vec3(camera.m_position))
    , m_target(glm::vec3(camera.m_target))
    , m_up(glm::vec3(camera.m_up))
    , m_forward(camera.m_target - camera.m_position)
    , m_right(- glm::cross(camera.m_up, m_forward))
    , m_speed(camera.m_speed)
    , m_fov(camera.m_fov)
    , m_yaw(90.f)
    , m_pitch(0.f)
    , m_near(camera.m_near)
    , m_far(camera.m_far)
{
    m_projection = glm::mat4(camera.m_projection);
}

glm::mat4 Camera::look_at()
{
    return glm::lookAt(m_position, m_position + m_forward, m_up);
}

float Camera::get_speed()
{
    return m_speed;
}

glm::vec3 Camera::get_position()
{
    return m_position;
}

glm::vec3 Camera::get_target()
{
    return m_target;
}

glm::vec3 Camera::get_up()
{
    return m_up;
}

glm::vec3 Camera::get_forward()
{
    return m_forward;
}

glm::vec3 Camera::get_right()
{
    return m_right;
}

float* Camera::get_fov()
{
    return &m_fov;
}

float* Camera::get_near()
{
    return &m_near;
}

float* Camera::get_far()
{
    return &m_far;
}

glm::mat4 Camera::get_projection()
{
    return glm::perspective(glm::radians(m_fov), 1920/1080.f, m_near, m_far);
}

void Camera::set_speed(float speed)
{
    m_speed = speed;
}

void Camera::set_position(glm::vec3 position)
{
    m_position = position;
}

void Camera::set_position_y(float y)
{
    m_position.y = y;
}

void Camera::set_target(glm::vec3 target)
{
    m_target = target;
}

void Camera::set_up(glm::vec3 up)
{
    m_up = up;
}

void Camera::set_forward(glm::vec3 forward)
{
    m_forward = forward;
}

void Camera::set_right(glm::vec3 right)
{
    m_right = right;
}

void Camera::update(GLFWwindow *window, float delta, float mouse_x, float mouse_y, bool move)
{
    m_move = move;
    // Keyboard events
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)  // W : forwards
    {
        set_position(m_position + m_forward * m_speed * delta);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)  // S : backwards
    {
        set_position(m_position - m_forward * m_speed * delta);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)  // A : left
    {
        set_position(m_position - m_right * m_speed * delta);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)  // D : right
    {
        set_position(m_position + m_right * m_speed * delta);
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)  // E : Up
    {
        set_position(m_position + m_up * m_speed * delta);
    }
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)  // Q : Down
    {
        set_position(m_position - m_up * m_speed * delta);
    }

    // Mouse events
    mouse_move(window, mouse_x, mouse_y);
}

void Camera::reset_mouse_pos()
{
    m_first_move = true;
}

void Camera::mouse_move(GLFWwindow* window, double xpos, double ypos)
{
    if (!m_move)
        return;
        
    m_mouse_pos.x = xpos;   
    m_mouse_pos.y = ypos;

    if (m_first_move)
    {
        m_last_mouse_pos = glm::vec2(xpos, ypos);
        m_first_move = false;
    }

    float offset_x = xpos - m_last_mouse_pos.x;
    float offset_y = m_last_mouse_pos.y - ypos;

    m_last_mouse_pos = glm::vec2(xpos, ypos);

    offset_x *= m_sensitivity;
    offset_y *= m_sensitivity;

    m_yaw   += offset_x;
    m_pitch += offset_y;

    if(m_pitch > 89.0f)
        m_pitch = 89.0f;
    if(m_pitch < -89.0f)
        m_pitch = -89.0f;

    m_forward.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    m_forward.y = sin(glm::radians(m_pitch));
    m_forward.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    m_forward = glm::normalize(m_forward);
    m_right = glm::normalize(glm::cross(m_forward, glm::vec3(0.f, 1.f, 0.f)));
    m_up = glm::normalize(glm::cross(m_right, m_forward));
}

void Camera::invert_pitch()
{
    m_pitch = - m_pitch;
    m_forward.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    m_forward.y = sin(glm::radians(m_pitch));
    m_forward.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    m_forward = glm::normalize(m_forward);
    m_right = glm::normalize(cross(m_forward, glm::vec3(0.f, 1.f, 0.f)));
    m_up = glm::normalize(cross(m_right, m_forward));
}
