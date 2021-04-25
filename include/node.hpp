#pragma once

#include <vector>
#include <memory>
#include <glm/glm.hpp>

#include <tiny_gltf.h>

#include "mesh.hpp"

using shared_program = std::shared_ptr<Program>;
using shared_mesh = std::shared_ptr<Mesh>;

class Node
{
public:
    Node(tinygltf::Node& node, tinygltf::Model& model, std::vector<shared_mesh> meshes);
    ~Node();

    void draw(const shared_program program, glm::mat4 transform = glm::mat4(1.0));
protected:
    void process_transform(tinygltf::Node& node);
    void process_mesh(tinygltf::Node& node, std::vector<shared_mesh> meshes);

    shared_mesh m_mesh;
    std::vector<std::shared_ptr<Node>> m_children;
    glm::mat4 m_transform;
};