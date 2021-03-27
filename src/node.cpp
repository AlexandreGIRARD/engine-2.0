#include "node.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

Node::Node(tinygltf::Node& node, tinygltf::Model& model, std::vector<shared_mesh> meshes)
{
    // Process element
    process_transform(node);
    process_mesh(node, meshes);

    m_children.reserve(node.children.size());
    for (auto child : node.children) {
        m_children.emplace_back(std::make_shared<Node>(model.nodes[child], model, meshes));
    }
}

Node::~Node()
{}

void Node::process_transform(tinygltf::Node& node)
{
    if (node.matrix.size() == 16) // case where matrix is specified 
    { 
        std::vector<double> mat = node.matrix;
        m_transform = glm::mat4(mat[0], mat[1], mat[2], mat[3],
                                mat[4], mat[5], mat[6], mat[7],
                                mat[8], mat[9], mat[10], mat[11],
                                mat[12], mat[13], mat[14], mat[15]);
    }
    else 
    {
        glm::mat4 T = glm::mat4(1.0);
        glm::mat4 R = glm::mat4(1.0);
        glm::mat4 S = glm::mat4(1.0);

        auto node_T = node.translation;
        if (!node_T.empty()) {
            T = glm::translate(T, glm::vec3(node_T[0], node_T[1], node_T[2]));
        }
        
        auto node_R = node.rotation;
        if (!node_R.empty()) {
            glm::quat quat = glm::quat(node_R[0], node_R[1], node_R[2], node_R[3]);
            R = glm::mat4_cast(quat);
        }

        auto node_S = node.scale;
        if (!node_S.empty()) {
            S = glm::scale(S, glm::vec3(node_S[0], node_S[1], node_S[2]));
        }

        m_transform = T * R * S;
    }
}

void Node::process_mesh(tinygltf::Node& node, std::vector<shared_mesh> meshes)
{
    if (node.mesh < 0 || node.mesh >= meshes.size())
        return;
    m_mesh = meshes[node.mesh];
}

void Node::draw(const Program& program, glm::mat4 transform)
{
    glm::mat4 new_transform = transform * m_transform;
    if (m_mesh)
        m_mesh->draw(program, new_transform);

    for (auto child : m_children)
        child->draw(program, new_transform);
}