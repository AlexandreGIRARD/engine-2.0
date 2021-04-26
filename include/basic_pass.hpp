#pragma once

#include "render_pass.hpp"

class Basic_Pass : public Render_Pass
{
public:
    Basic_Pass();
    ~Basic_Pass();

    void render(Camera* camera, Scene* scene) override;
    const std::vector<shared_attachment> get_attachments() override;
};