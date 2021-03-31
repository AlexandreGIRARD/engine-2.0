#pragma once

#include <vector>
#include <string>
#include <glm/glm.hpp>

class Program
{
public:
    Program(bool material_binding);
    ~Program();

    const bool need_material_binding();

    void add_shader(const std::string& path, int shader_type);
    void link();
    void use();
    void addUniformMat4(const glm::mat4& matrix, const char *name) const;
    void addUniformVec2(const glm::vec2& vector, const char *name) const;
    void addUniformVec3(const glm::vec3& vector, const char *name) const;
    void addUniformVec4(const glm::vec4& vector, const char *name) const;
    void addUniformTexture(const uint unit, const char *name) const;
    void addUniformFloat(const float val, const char* name) const;
    void addUniformUint(const uint val, const char* name) const;

private:
        uint m_id_program;
        std::vector<uint> m_shaders;

        bool m_material_binding;
};