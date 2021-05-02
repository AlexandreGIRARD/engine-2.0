#pragma once

#include <vector>
#include <memory>

#include "program.hpp"
#include "camera.hpp"
#include "scene.hpp"
#include "fbo.hpp"

using shared_camera  = std::shared_ptr<Camera>;
using shared_scene   = std::shared_ptr<Scene>;
using shared_fbo     = std::shared_ptr<FBO>;
using shared_program = std::shared_ptr<Program>;

class Render_Pass
{
public:
    Render_Pass(unsigned int width, unsigned int height, bool bind_material)
        : m_width(width)
        , m_height(height)
    {
        m_program = std::make_shared<Program>(bind_material);
        m_fbo     = std::make_shared<FBO>();
    }
    ~Render_Pass()
    {}

    virtual void render(Camera* camera, Scene* scene) = 0;
    virtual const std::vector<shared_attachment> get_attachments() = 0;
    virtual void resize(unsigned int width, unsigned int height)
    {
        m_width = width;
        m_height = height;
    }

    const shared_fbo get_fbo() { return m_fbo; }

protected:
    shared_program m_program;
    shared_fbo     m_fbo;

    unsigned int m_width;
    unsigned int m_height;
};