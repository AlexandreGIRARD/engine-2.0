# PBR Engine

This is my second OpenGL engine where the purpose of this project is to learn other rendering techniques and improve both my C++ and OpenGL knowledge.

It's a deferred rendering engine based on the glTF file format. 

## Features

### Implemented
- Physically Based Rendering
  - Metallic Roughness Workflow
  - IBL
- Skybox
- Screen Space Ambient Occlusion
- Fast Approximate Anti-Aliasing
- Debug Mode
- FX
  - Bloom
  - Depth of Field
  - Multiple Tone Mapping

### Need to be transfered from the old Engine
- Shadow Mapping (Both Directional and Point)
- Relief Mapping
- Water Rendering

### Coming Soon
See [issues](https://github.com/AlexandreGIRARD/engine-2.0/issues)
- Specular Glossiness Workflow
- Post Fx effects
  - Grain
  - Chromatic Aberration
  - Lens-Flare  
- Other Anti-Aliasing Algorithms
- Other Ambien Occlusion Algorithms


## Libraries
- [GLFW](https://www.glfw.org/) --> OpenGL context and window API
- [GLAD](https://github.com/Dav1dde/glad) --> OpenGL symbols loader
- [tinygltf](https://github.com/syoyo/tinygltf) --> glTF loader
- [GLM](https://glm.g-truc.net/0.9.2/api/index.html) --> Mathematic library
- [STB](https://github.com/nothings/stb) --> Image loader


## Results
[Youtube Video](https://www.youtube.com/watch?v=GBET792NKIw)
