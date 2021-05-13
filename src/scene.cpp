#include "scene.hpp"

// Define these only in *one* .cc file.
#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <tiny_gltf.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

// I choose to separate them from the node architecture
void Scene::init_lights(tinygltf::Model& model, tinygltf::Scene& scene)
{
    // If exported from blender the node n has the transform and node n - 1 the light information
    for (auto node: scene.nodes) {
        const auto& current_node = model.nodes[node];
        if (!current_node.children.size()) // Doesn't have children so discard
            continue;
        const auto& light_node = model.nodes[current_node.children[0]];

        const auto& it = light_node.extensions.find("KHR_lights_punctual");
        if (it == light_node.extensions.end() || !it->second.IsObject()) 
            continue;
        
        shared_light light = m_lights[it->second.Get("light").GetNumberAsInt()];

        // Can create the lights
        if (light->get_type() == Light_Type::POINT_LIGHT) 
        {
            light->set_position(glm::vec3{current_node.translation[0], current_node.translation[1], current_node.translation[2]});
        }
        else
        {
            glm::quat quat = glm::quat(current_node.rotation[3], current_node.rotation[0], current_node.rotation[1], current_node.rotation[2]);
            glm::mat4 rotation_matrix = glm::mat4_cast(quat);
            glm::vec4 direction = rotation_matrix * glm::vec4{0.f,0.f,-1.f,0.f};

            light->set_direction(glm::vec3{direction});
        }
    }
}

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

    // Create Lights
    for (auto light : model.lights) {
        m_lights.emplace_back(std::make_shared<Light>(light));
    }
    init_lights(model, scene);


    // Create Nodes
    for (auto node: scene.nodes) {
        m_nodes.emplace_back(std::make_shared<Node>(model.nodes[node], model, m_meshes));
    }
    return true;
}

Scene::Scene(const char* path)
{
    tinygltf::Model model;
    tinygltf::TinyGLTF loader;

    std::string err;
    std::string warn;

    if (std::string(path).find(".glb") != std::string::npos)
        loader.LoadBinaryFromFile(&model, &err, &warn, path);
    else
        loader.LoadASCIIFromFile(&model, &err, &warn, path);
        
    if (!warn.empty()) {
        printf("Warn: %s\n", warn.c_str());
    }

    if (!err.empty()) {
        printf("Err: %s\n", err.c_str());
    }

    create_scene(model);
}

void Scene::draw(const shared_program program)
{
    for (auto node : m_nodes)
        node->draw(program);
}

std::vector<shared_light> Scene::get_lights()
{
    return m_lights;
}

shared_light Scene::get_sunlight()
{
    for (auto light : m_lights)
    {
        if (light->get_type() == Light_Type::DIRECTIONAL_LIGHT)
            return light;
    }
    return nullptr;
}