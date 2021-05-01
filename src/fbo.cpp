#include "fbo.hpp"

#include <glad/glad.h>

Attachment::Attachment(unsigned int target, unsigned int width,unsigned int height, unsigned int format, unsigned int type)
    : m_target(target)
    , m_width(width)
    , m_height(height)
    , m_format(format)
    , m_type(type)
{
    glGenTextures(1, &m_name);
    glBindTexture(m_target, m_name);
    glTexParameteri(m_target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(m_target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(m_target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(m_target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    if (m_target == GL_TEXTURE_2D)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, m_format, m_width, m_height, 0, m_format, m_type, nullptr);
    }
    else
    {
        for (int i = 0; i < 6; i++)
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, m_format, m_width, m_height, 0, m_format, m_type, nullptr);
    }

}

Attachment::~Attachment()
{
    glDeleteTextures(1, &m_name);
}

FBO::FBO()
{
    glGenFramebuffers(1, &m_name);
    glBindFramebuffer(GL_FRAMEBUFFER, m_name);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

FBO::~FBO()
{
    glDeleteFramebuffers(1, &m_name);
}

const void FBO::bind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_name);
}

const void FBO::unbind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

const void FBO::set_attachment(shared_attachment attachment, unsigned int attachment_layout)
{
    glFramebufferTexture2D(GL_FRAMEBUFFER, attachment_layout, GL_TEXTURE_2D, attachment->m_name, 0);
}