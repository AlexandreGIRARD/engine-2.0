#pragma once

#include "render_pass.hpp"
#include "fbo.hpp"

class G_Buffer_Pass : public Render_Pass
{
public :
    G_Buffer_Pass(unsigned int width, unsigned int height);
    ~G_Buffer_Pass();

    void render(Camera* camera, Scene* scene) override;

    shared_attachment m_attach_position; // Position Attachment - World Space
    shared_attachment m_attach_albedo; // Albedo Attachment
    shared_attachment m_attach_orm; // Ambient Occlusion Metallic Roughness  Attachment
    shared_attachment m_attach_emissive; // Emissive Attachment
    shared_attachment m_attach_normal; // Normal Attachment - World Space
    shared_attachment m_attach_depth; // Depth Attachment
};