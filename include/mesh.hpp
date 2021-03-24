#pragma once

#include <memory>
#include <tiny_gltf.h>

#include "material.hpp"
#include "program.hpp"

using shared_material = std::shared_ptr<Material>;

struct ebo_t
{
    uint name;
    size_t count;
    int type;
    size_t offset;
};

struct Primitive
{
    Primitive(const tinygltf::Primitive& primitive, const tinygltf::Model& model, const std::vector<shared_material> materials);
    ~Primitive();

    void draw(Program& program);

    bool m_is_indexed = false;
    ebo_t m_ebo;

    uint m_vao = -1;
    std::vector<uint> m_vbos;

    uint m_mode;

    shared_material m_material;
};

class Mesh
{
public:
    Mesh(const tinygltf::Mesh& mesh, const tinygltf::Model& model, const std::vector<shared_material> materials);
    ~Mesh();

    void draw(Program& program);

private:
    std::vector<std::shared_ptr<Primitive>> m_primitives;
};