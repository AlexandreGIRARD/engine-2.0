#pragma once

#include "render_pass.hpp"

class Basic_Pass : public Render_Pass
{
public:
    Basic_Pass();
    ~Basic_Pass();

    void render(Camera* camera, Scene* scene) override;
};