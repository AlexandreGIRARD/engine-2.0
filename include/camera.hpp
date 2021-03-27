#pragma once

#include <memory>
#include <tiny_gltf.h>

#include <glm/glm.hpp>

class GLFWwindow;

class Camera
{
public:

    Camera()
    {}
    Camera(glm::vec3 position, glm::vec3 target, glm::vec3 up, float fov, float speed, float near, float far, float ratio);
    Camera(glm::vec3 position, glm::vec3 target, glm::vec3 up);
    Camera(const Camera &cam);

    glm::mat4 look_at();

    float get_speed();
    glm::vec3 get_position();
    glm::vec3 get_target();
    glm::vec3 get_up();
    glm::vec3 get_forward();
    glm::vec3 get_right();
    glm::mat4 get_projection();

    void set_speed(float speed);
    void set_position(glm::vec3 position);
    void set_position_y(float y);
    void set_target(glm::vec3 target);
    void set_up(glm::vec3 up);
    void set_forward(glm::vec3 forward);
    void set_right(glm::vec3 right);

    void update(GLFWwindow *window, float delta, float mouse_x, float mouse_y);
    void mouse_move(double xpos, double ypos);
    void invert_pitch();

private:
    glm::vec3 m_position;
    glm::vec3 m_target;
    glm::vec3 m_up;
    glm::vec3 m_forward;
    glm::vec3 m_right;

    float m_speed;
    float m_fov;
    float m_yaw;
    float m_pitch;
    float m_near;
    float m_far;

    glm::mat4 m_projection;

    glm::vec2 m_last_mouse_pos;
    glm::vec2 m_mouse_pos;
    bool m_first_move = true;
    float m_sensitivity = 0.1f;
};