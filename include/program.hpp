#pragma once

#include <vector>
#include <string>
#include <glm/glm.hpp>

class Program
{
public:
    Program();
    ~Program();

    void add_shader(std::string path, int shader_type);
    void link();
    void use();
    void addUniformMat4(const glm::mat4& matrix, const char *name);\
    void addUniformVec2(const glm::vec2& vector, const char *name);
    void addUniformVec3(const glm::vec3& vector, const char *name);
    void addUniformVec4(const glm::vec4& vector, const char *name);
    void addUniformTexture(const uint unit, const char *name);
    void addUniformFloat(const float val, const char* name);
    void addUniformUint(const uint val, const char* name);

private:
        uint m_id_program;
        std::vector<uint> m_shaders;
};