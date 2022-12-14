#pragma once

#include "glad/glad.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

class shader
{
public:
    unsigned int id;

    shader(const char* vertex_path, const char* fragment_path, const char* geometry_path = nullptr);

    void use() const;

    void set_bool(const std::string& name, bool value) const;
    void set_int(const std::string& name, int value) const;
    void set_float(const std::string& name, float value) const;
    void set_mat4(const std::string& name, glm::mat4 value) const;
    void set_vec3(const std::string& name, float x, float y, float z) const;
    void set_vec3(const std::string& name, glm::vec3 value) const;
    void set_vec2(const std::string& name, float x, float y) const;

private:
    void compile_shader(const char* vertex_shader_code, const char* fragment_shader_code,
                        const char* geometry_shader_code = nullptr);
};

inline std::string shader_read_all_lines(const std::string& path)
{
    std::ifstream file;
    constexpr int exception_mask = std::ifstream::failbit | std::ifstream::badbit;
    file.exceptions(exception_mask);

    file.open(path);
    std::ostringstream output_stream;
    output_stream << file.rdbuf();
    std::string result = output_stream.str();

    return result;
}

inline void shader::compile_shader(const char* vertex_shader_code, const char* fragment_shader_code,
                                   const char* geometry_shader_code)
{
    int success;
    constexpr size_t info_log_size = 512;
    char info_log[info_log_size];

    const unsigned int vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vertex_shader_code, nullptr);
    glCompileShader(vertex);

    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertex, info_log_size, nullptr, info_log);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << info_log << std::endl;
    }

    const unsigned int fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fragment_shader_code, nullptr);
    glCompileShader(fragment);

    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragment, info_log_size, nullptr, info_log);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << info_log << std::endl;
    }

    unsigned int geometry;
    if (geometry_shader_code)
    {
        geometry = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(geometry, 1, &geometry_shader_code, nullptr);
        glCompileShader(geometry);

        glGetShaderiv(geometry, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(geometry, info_log_size, nullptr, info_log);
            std::cout << "ERROR::SHADER::GEOMETRY::COMPILATION_FAILED\n" << info_log << std::endl;
        }
    }
    else
    {
        geometry = 0;
    }


    id = glCreateProgram();
    glAttachShader(id, vertex);
    glAttachShader(id, fragment);
    if (geometry_shader_code)
        glAttachShader(id, geometry);
    glLinkProgram(id);

    glGetProgramiv(id, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(id, info_log_size, nullptr, info_log);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << info_log << std::endl;
    }

    glDeleteShader(vertex);
    glDeleteShader(fragment);
    if (geometry_shader_code)
        glAttachShader(id, geometry);
}

inline shader::shader(const char* vertex_path, const char* fragment_path, const char* geometry_path)
{
    std::string vertex_code;
    std::string fragment_code;
    std::string geometry_code;

    try
    {
        vertex_code = shader_read_all_lines(vertex_path);
        fragment_code = shader_read_all_lines(fragment_path);
        if (geometry_path)
            geometry_code = shader_read_all_lines(geometry_path);
    }
    catch (const std::ifstream::failure&)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << std::endl;
    }

    const char* vertex_shader_code = vertex_code.c_str();
    const char* fragment_shader_code = fragment_code.c_str();
    const char* geometry_shader_code = geometry_path ? geometry_code.c_str() : nullptr;

    id = 0;
    compile_shader(vertex_shader_code, fragment_shader_code, geometry_shader_code);
}

inline void shader::use() const
{
    glUseProgram(id);
}

inline void shader::set_bool(const std::string& name, const bool value) const
{
    glUniform1i(glGetUniformLocation(id, name.c_str()), static_cast<int>(value));
}

inline void shader::set_int(const std::string& name, const int value) const
{
    glUniform1i(glGetUniformLocation(id, name.c_str()), value);
}

inline void shader::set_float(const std::string& name, const float value) const
{
    glUniform1f(glGetUniformLocation(id, name.c_str()), value);
}

inline void shader::set_mat4(const std::string& name, glm::mat4 value) const
{
    glUniformMatrix4fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, value_ptr(value));
}

inline void shader::set_vec3(const std::string& name, const float x, const float y, const float z) const
{
    glUniform3f(glGetUniformLocation(id, name.c_str()), x, y, z);
}

inline void shader::set_vec3(const std::string& name, const glm::vec3 value) const
{
    glUniform3f(glGetUniformLocation(id, name.c_str()), value.x, value.y, value.z);
}

inline void shader::set_vec2(const std::string& name, const float x, const float y) const
{
    glUniform2f(glGetUniformLocation(id, name.c_str()), x, y);
}
