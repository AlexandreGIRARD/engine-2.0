#include "node.hpp"

Node::Node(tinygltf::Node& node, tinygltf::Model& model)
{
    // Process element

    m_children.reserve(node.children.size());
    for (auto child : node.children) {
        m_children.emplace_back(std::make_shared<Node>(model.nodes[child], model));
    }
}

void Node::process_transform(tinygltf::Node& node)
{
    if (node.matrix.size() == 16) { // case where matrix is specified
        std::vector<double> mat = node.matrix;
        m_transform = glm::mat4(mat[0], mat[1], mat[2], mat[3],
                                mat[4], mat[5], mat[6], mat[7],
                                mat[8], mat[9], mat[10], mat[11],
                                mat[12], mat[13], mat[14], mat[15],)
    }
}

void process_mesh(tinygltf::Node& node, tinygltf::Model& model)
{

}