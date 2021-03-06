#pragma once

#include <memory>

struct Attachment
{
    Attachment(unsigned int target, unsigned int width,unsigned int height, unsigned int internal_format, unsigned int format, unsigned int type);
    ~Attachment();
    void set_size(unsigned int width, unsigned int height);
    void resize(unsigned int width, unsigned int height);
    void generate_mipmap();

    unsigned int m_target;
    unsigned int m_width;
    unsigned int m_height;
    unsigned int m_internal_format;
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

    const void set_attachment(shared_attachment attachment, unsigned int attachment_layout, int mip_level = 0);

    const int get_name() { return m_name; };

private:
    unsigned int m_name;
    
};