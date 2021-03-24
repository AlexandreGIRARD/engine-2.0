#include "program.hpp"

#include <fstream>
#include <sstream>
#include <iostream>

#include <glad/glad.h>

#include <glm/gtc/type_ptr.hpp>

Program::Program(bool material_binding)
{
    m_id_program = glCreateProgram();
    m_material_binding = material_binding;
}

Program::~Program()
{
    glDeleteProgram(m_id_program);   
}

const bool Program::need_material_binding()
{
    return m_material_binding;
}

void Program::add_shader(std::string path, int shader_type)
{
    // Read file
    std::ifstream shader("shaders/" + path);
    std::stringstream buffer;
    buffer << shader.rdbuf();
    shader.close();
    std::string tmp_string = buffer.str();
    const char *code = tmp_string.c_str();

    // Create shader
    uint id = glCreateShader(shader_type);
    glShaderSource(id, 1, &code, NULL);
    glCompileShader(id);

    int success;
    char infoLog[512];
    glGetShaderiv(id, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        glGetShaderInfoLog(id, 512, NULL, infoLog);
        std::cout << "Error at shader compilation: " << path << "\n" << infoLog << std::endl;
        exit(-1);
    }

    glAttachShader(m_id_program, id);
    glDeleteShader(id);

    m_shaders.emplace_back(id);
}

void Program::link()
{
    glLinkProgram(m_id_program);

    GLint isLinked = 0;
    glGetProgramiv(m_id_program, GL_LINK_STATUS, &isLinked);
    if (isLinked == GL_FALSE)
    {
        GLint maxLength = 0;
        glGetProgramiv(m_id_program, GL_INFO_LOG_LENGTH, &maxLength);

        // The maxLength includes the NULL character
        char *infoLog = (char*)std::malloc(maxLength+1);
        glGetProgramInfoLog(m_id_program, maxLength, &maxLength, infoLog);
        std::cout << infoLog << std::endl;
        std::free(infoLog);

        // The program is useless now. So delete it.
        glDeleteProgram(m_id_program);

        // Provide the infolog in whatever manner you deem best.
        // Exit with failure.
        exit(1);
    }
}

void Program::use()
{
    glUseProgram(m_id_program);
}

void Program::addUniformTexture(const uint unit, const char *name)
{
    glUniform1i(glGetUniformLocation(m_id_program, name), unit);
}

void Program::addUniformVec2(const glm::vec2& vector, const char *name)
{
    glUniform2f(glGetUniformLocation(m_id_program, name), vector.x, vector.y);
}

void Program::addUniformVec3(const glm::vec3& vector, const char *name)
{
    glUniform3f(glGetUniformLocation(m_id_program, name), vector.x, vector.y, vector.z);
}

void Program::addUniformVec4(const glm::vec4& vector, const char *name)
{
    glUniform4f(glGetUniformLocation(m_id_program, name), vector.x, vector.y, vector.z, vector.w);
}

void Program::addUniformMat4(const glm::mat4& matrix, const char *name)
{
    int location = glGetUniformLocation(m_id_program, name);
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}

void Program::addUniformUint(const uint val, const char* name)
{
    glUniform1ui(glGetUniformLocation(m_id_program, name), val);
}

void Program::addUniformFloat(const float val, const char* name)
{
    glUniform1f(glGetUniformLocation(m_id_program, name), val);
}
