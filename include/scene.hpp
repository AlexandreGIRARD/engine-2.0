#pragma once

// Personal renderer classes
#include "node.hpp"
#include "mesh.hpp"
#include "material.hpp"
#include "light.hpp"
#include "program.hpp"

using shared_program = std::shared_ptr<Program>;
using shared_node = std::shared_ptr<Node>;
using shared_material = std::shared_ptr<Material>;
using shared_light = std::shared_ptr<Light>;
using shared_mesh = std::shared_ptr<Mesh>;

class Scene
{
public:
    Scene(){}
    Scene(const char* path);

    void draw(const shared_program program);

private:
    bool create_scene(tinygltf::Model& model);
    void init_lights(tinygltf::Model& model, tinygltf::Scene& scene);


    std::vector<shared_material> m_materials;
    std::vector<shared_mesh>     m_meshes;
    std::vector<shared_node>     m_nodes;
    std::vector<shared_light>    m_lights; // Only Point/Spot
    shared_light                 m_sun_light;
};