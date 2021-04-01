#pragma once

#include <memory>
#include <tiny_gltf.h>

#include "material.hpp"
#include "program.hpp"

using shared_material = std::shared_ptr<Material>;

struct ebo_t
{
    unsigned int name;
    size_t count;
    int type;
    size_t offset;
};

struct Primitive
{
    Primitive(const tinygltf::Primitive& primitive, const tinygltf::Model& model, const std::vector<shared_material> materials);
    ~Primitive();

    void draw(const Program& program);

    bool m_is_indexed = false;
    ebo_t m_ebo;

    unsigned int m_vao = -1;
    std::vector<unsigned int> m_vbos;

    unsigned int m_mode;

    shared_material m_material;
};

class Mesh
{
public:
    Mesh(const tinygltf::Mesh& mesh, const tinygltf::Model& model, const std::vector<shared_material> materials);
    ~Mesh();

    void draw(const Program& program, const glm::mat4& transform);

private:
    std::vector<std::shared_ptr<Primitive>> m_primitives;
};