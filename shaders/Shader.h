#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>
#include <glm/gtc/type_ptr.hpp>

#include <fstream>
#include <sstream>
#include <iostream>

class Shader
{
public:
    Shader(const char* vertexPath, const char* fragmentPath)
    {
        std::string vertexSource = LoadFile(vertexPath);
        std::string fragmentSource = LoadFile(fragmentPath);

        GLuint vertexShader = CompileShader(GL_VERTEX_SHADER, vertexSource);
        GLuint fragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentSource);

        if (vertexShader != 0 && fragmentShader != 0)
        {
            LinkProgram(vertexShader, fragmentShader);
        }

        if (vertexShader != 0)
            glDeleteShader(vertexShader);

        if (fragmentShader != 0)
            glDeleteShader(fragmentShader);
    }

    ~Shader()
    {
        if (ID != 0)
            glDeleteProgram(ID);
    }

    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    void Use() const
    {
        glUseProgram(ID);
    }

    void SetMat4(const std::string& name, const glm::mat4& value) const
    {
        GLint location = glGetUniformLocation(ID, name.c_str());

        if (location != -1)
            glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
    }

    void SetVec3(const std::string& name, const glm::vec3& value) const
    {
        GLint location = glGetUniformLocation(ID, name.c_str());

        if (location != -1)
            glUniform3fv(location, 1, glm::value_ptr(value));
    }

    void SetFloat(const std::string& name, float value) const
    {
        GLint location = glGetUniformLocation(ID, name.c_str());

        if (location != -1)
            glUniform1f(location, value);
    }

    void SetInt(const std::string& name, int value) const
    {
        GLint location = glGetUniformLocation(ID, name.c_str());

        if (location != -1)
            glUniform1i(location, value);
    }

private:
    GLuint ID = 0;

    std::string LoadFile(const char* path) const
    {
        std::ifstream file(path);

        if (!file.is_open())
        {
            std::cerr << "Failed to open shader file: " << path << '\n';
            return {};
        }

        std::stringstream stream;
        stream << file.rdbuf();

        return stream.str();
    }

    GLuint CompileShader(GLenum type, const std::string& source) const
    {
        if (source.empty())
        {
            std::cerr << "Shader source is empty.\n";
            return 0;
        }

        GLuint shader = glCreateShader(type);

        const char* sourcePtr = source.c_str();
        glShaderSource(shader, 1, &sourcePtr, nullptr);
        glCompileShader(shader);

        GLint success = GL_FALSE;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

        if (!success)
        {
            GLint logLength = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);

            std::string log(logLength, '\0');
            glGetShaderInfoLog(shader, logLength, nullptr, log.data());

            std::cerr << "Shader compilation failed:\n"
                << log << '\n';

            glDeleteShader(shader);
            return 0;
        }

        return shader;
    }

    void LinkProgram(GLuint vertexShader, GLuint fragmentShader)
    {
        ID = glCreateProgram();

        glAttachShader(ID, vertexShader);
        glAttachShader(ID, fragmentShader);

        glLinkProgram(ID);

        GLint success = GL_FALSE;
        glGetProgramiv(ID, GL_LINK_STATUS, &success);

        if (!success)
        {
            GLint logLength = 0;
            glGetProgramiv(ID, GL_INFO_LOG_LENGTH, &logLength);

            std::string log(logLength, '\0');
            glGetProgramInfoLog(ID, logLength, nullptr, log.data());

            std::cerr << "Shader linking failed:\n"
                << log << '\n';

            glDeleteProgram(ID);
            ID = 0;
        }
    }
};
