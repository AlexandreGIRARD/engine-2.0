#pragma once

// Personal renderer classes
#include "node.hpp"
#include "mesh.hpp"
#include "material.hpp"
#include "light.hpp"

using shared_node = std::shared_ptr<Node>;
using shared_material = std::shared_ptr<Material>;
using shared_light = std::shared_ptr<Light>;
using shared_mesh = std::shared_ptr<Mesh>;

class Scene
{
public:
    Scene(const char* path);

    void draw();

private:
    bool create_scene(tinygltf::Model& model);


    std::vector<shared_material> m_materials;
    std::vector<shared_mesh>     m_meshes;
    std::vector<shared_node>     m_nodes;
    std::vector<shared_light>    m_lights;
};