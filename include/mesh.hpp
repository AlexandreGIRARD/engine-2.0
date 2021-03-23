#pragma once

#include <memory>
#include <tiny_gltf.h>

#include "material.hpp"

using shared_material = std::shared_ptr<Material>;

struct ebo_t
{
    uint name;
    uint count;
    uint type;
    uint offset;
};

struct Primitive
{
    Primitive(const tinygltf::Primitive& primitive, const tinygltf::Model& model, const std::vector<shared_material> materials);
    ~Primitive();

    void draw();

    bool m_is_indexed = false;
    ebo_t m_ebo;

    uint m_vao = -1;
    std::vector<uint> m_vbos;

    GLenum m_mode;

    shared_material m_material;
};

class Mesh
{
public:
    Mesh(const tinygltf::Mesh& mesh, const tinygltf::Model& model, const std::vector<shared_material> materials);
    ~Mesh();

    void draw();

private:
    std::vector<std::shared_ptr<Primitive>> m_primitives;
};