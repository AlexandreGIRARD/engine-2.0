#pragma once

#include <vector>
#include <memory>
#include <glm/glm.hpp>

#include <tiny_gltf.h>

#include "mesh.hpp"

using shared_mesh = std::shared_ptr<Mesh>;

class Node
{
public:
    Node(tinygltf::Node& node, tinygltf::Model& model);
    ~Node();

    void draw(Program& program);
protected:
    void process_transform(tinygltf::Node& node);
    void process_mesh(tinygltf::Node& node, tinygltf::Model& model);

    shared_mesh m_mesh;
    std::vector<std::shared_ptr<Node>> m_children;
    glm::mat4 m_transform;
};