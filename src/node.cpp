#include "node.hpp"

Node::Node(tinygltf::Node& node_t, tinygltf::Model& model_t)
{
    // Process element

    m_children.reserve(node_t.children.size());
    for (auto child : node_t.children) {
        // m_children.emplace_back(model_t.nodes[child], model_t);
    }
}