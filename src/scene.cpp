#include "scene.hpp"

// Define these only in *one* .cc file.
#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <tiny_gltf.h>

bool Scene::create_scene(tinygltf::Model& model)
{
    // temporary but will display first scene
    tinygltf::Scene scene = model.scenes[0];
    
    // Create Materials
    for (auto material : model.materials) {
        m_materials.emplace_back(std::make_shared<Material>(material, model));
    }

    // Create Meshes
    for (auto mesh : model.meshes) {
        m_meshes.emplace_back(std::make_shared<Mesh>(mesh, model, m_materials));
    }
    

    // Create Nodes
    for (auto node: scene.nodes) {
        m_nodes.emplace_back(std::make_shared<Node>(model.nodes[node], model));
    }
    return true;
}

Scene::Scene(const char* path)
{
    tinygltf::Model model;
    tinygltf::TinyGLTF loader;

    std::string err;
    std::string warn;

    loader.LoadBinaryFromFile(&model, &err, &warn, path);

    if (!warn.empty()) {
        printf("Warn: %s\n", warn.c_str());
    }

    if (!err.empty()) {
        printf("Err: %s\n", err.c_str());
    }

    create_scene(model);
}

void Scene::draw()
{

}