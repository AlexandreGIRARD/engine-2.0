#pragma once

#include <memory>

struct Attachment
{
    Attachment(unsigned int width,unsigned int height, unsigned int format, unsigned int type);
    ~Attachment();

    unsigned int m_width;
    unsigned int m_height;
    unsigned int m_format;
    unsigned int m_type;
    unsigned int m_name;
};

using shared_attachment = std::shared_ptr<Attachment>;

class FBO
{
public:
    FBO();
    ~FBO();

    const void bind();
    const void unbind();

    const void set_attachment(shared_attachment attachment, unsigned int attachment_layout);

    const int get_name() { return m_name; };

private:
    unsigned int m_name;
};