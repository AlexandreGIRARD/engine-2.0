#pragma once

#include <vector>
#include <memory>
#include <glm/glm.hpp>

#include <tiny_gltf.h>

class Node
{
public:
    Node(tinygltf::Node& node_t, tinygltf::Model& model_t);
    ~Node();
protected:
    std::vector<std::shared_ptr<Node>> m_children;
    glm::mat4 m_transformMatrix;
};