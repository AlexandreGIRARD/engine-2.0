#include <iostream>
#include <cmath>
#include <filesystem>
#include <memory>

#include "renderer.hpp"
#include "utils.hpp"

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cerr << "Usage: ./engine <scene.glb>" << std::endl;
        exit(1);
    }

    Renderer renderer = Renderer(4, 5, argv[1]);
    renderer.loop();

    return 0;
}
