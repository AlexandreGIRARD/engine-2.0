#include "mesh.hpp"

#include <glad/glad.h>

static const GLenum modes[] = {
    GL_POINT,           // TINYGLTF_MODE_POINT
    GL_LINE,            // TINYGLTF_MODE_LINE
    GL_LINE_LOOP,       // TINYGLTF_MODE_LOOP
    GL_LINE_STRIP,      // TINYGLTF_MODE_LINE_STRIP
    GL_TRIANGLES,       // TINYGLTF_MODE_TRIANGLES
    GL_TRIANGLE_STRIP,  // TINYGLTF_MODE_TRIANGLE_STRIP
    GL_TRIANGLE_FAN     // TINYGLTF_MODE_TRAINGLE_FAN
};


static uint gen_vbo(const tinygltf::Accessor& accessor, const tinygltf::Model& model, uint index)
{
    uint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    
    auto buffer_view = model.bufferViews[accessor.bufferView];
    auto buffer      = model.buffers[buffer_view.buffer];

    glBufferData(GL_ARRAY_BUFFER, buffer_view.byteLength, &buffer.data.at(0) + buffer_view.byteOffset, GL_STATIC_DRAW);
    int byteStride = accessor.ByteStride(model.bufferViews[accessor.bufferView]);

    int size = 1;
    if (accessor.type != TINYGLTF_TYPE_SCALAR) {
        size = accessor.type;
    }

    glEnableVertexAttribArray(index);
    glVertexAttribPointer(index, size, accessor.componentType, accessor.normalized ? true : false, byteStride, (void*)accessor.byteOffset);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return vbo;
}

static uint gen_ebo(const tinygltf::Accessor& accessor, const tinygltf::Model& model)
{
    uint ebo;
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

    auto buffer_view = model.bufferViews[accessor.bufferView];
    auto buffer      = model.buffers[buffer_view.buffer];

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, buffer_view.byteLength, &buffer.data.at(0) + buffer_view.byteOffset, GL_STATIC_DRAW);

    return ebo;
}


Primitive::Primitive(const tinygltf::Primitive& primitive, const tinygltf::Model& model, const std::vector<shared_material> materials)
{
    m_material = materials[primitive.material];
    m_mode     = modes[primitive.mode];

    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    if (primitive.indices != 1) {
        m_is_indexed = true;
        m_ebo.name   = gen_ebo(model.accessors[primitive.indices], model);
        m_ebo.count  = model.accessors[primitive.indices].count;
        m_ebo.type   = model.accessors[primitive.indices].componentType;
        m_ebo.offset = model.accessors[primitive.indices].byteOffset; 
    }

    for (const auto& attr : primitive.attributes) {
        int index = -1;
        if (attr.first.compare("POSITION") == 0)   index = 0;
        if (attr.first.compare("NORMAL") == 0)     index = 1;
        if (attr.first.compare("TANGENT") == 0)    index = 2;
        if (attr.first.compare("TEXCOORD_0") == 0) index = 3;
        if (attr.first.compare("TEXCOORD_1") == 0) index = 4;
        if (attr.first.compare("TEXCOORD_2") == 0) index = 5;
        if (attr.first.compare("TEXCOORD_3") == 0) index = 6;
        if (attr.first.compare("TEXCOORD_4") == 0) index = 7;
        if (attr.first.compare("TEXCOORD_5") == 0) index = 8;

        if (index != -1)
            m_vbos.emplace_back(gen_vbo(model.accessors[attr.second], model, index));
    }
    glBindVertexArray(0);
}

Primitive::~Primitive()
{
    glDeleteBuffers(m_vbos.size(), m_vbos.data());
    glDeleteBuffers(1, &m_vao);
    glDeleteBuffers(1, &m_ebo.name);
}

void Primitive::draw()
{
    glBindVertexArray(m_vao);

    if (m_is_indexed) {
        glDrawElements(m_mode, m_ebo.count, m_ebo.type, (void*)m_ebo.offset);
    } else {
        glDrawArrays(m_mode, 0, 0); // debug that
    }

    glBindVertexArray(0);
}

Mesh::Mesh(const tinygltf::Mesh& mesh, const tinygltf::Model& model, const std::vector<shared_material> materials)
{
    for (const auto& primitive : mesh.primitives) {
        m_primitives.emplace_back(std::make_shared<Primitive>(primitive, model, materials));
    }
}

Mesh::~Mesh()
{}

void Mesh::draw(const Program& program, const glm::mat4& transform)
{
    program.addUniformMat4(transform, "model");

    for (auto primitive : m_primitives) {
        primitive->draw();
    }
}