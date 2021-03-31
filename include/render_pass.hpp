#pragma once

#include <vector>
#include <memory>

#include "program.hpp"
#include "camera.hpp"
#include "scene.hpp"

using shared_camera = std::shared_ptr<Camera>;
using shared_scene  = std::shared_ptr<Scene>;

class Render_Pass
{
public:
    Render_Pass(bool bind_material)
        : m_program(bind_material)
    {}
    ~Render_Pass()
    {}

    virtual void render(shared_camera camera, shared_scene scene) = 0;
    virtual void set_attachments() = 0;
    virtual void link_attachments(std::vector<unsigned int> attachments) = 0;
    std::vector<unsigned int> get_attachments() { return m_attachments; }
protected:
    std::vector<unsigned int> m_attachments;
    Program m_program;
};