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
    Render_Pass(bool bind_material)
    {
        m_program = std::make_shared<Program>(bind_material);
        m_fbo     = std::make_shared<FBO>();
    }
    ~Render_Pass()
    {}

    virtual void render(Camera* camera, Scene* scene) = 0;

    const shared_fbo get_fbo() { return m_fbo; }

protected:
    shared_program m_program;
    shared_fbo     m_fbo;
};