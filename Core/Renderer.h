#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include "Environment/Mesh.h"
#include "shaders/Shader.h"

class Renderer
{
public:
    void Draw(
        const Mesh& mesh,
        const Shader& shader,
        const glm::mat4& model,
        const glm::mat4& view,
        const glm::mat4& projection,
        const glm::vec3& viewPos,
        const glm::vec3& color = glm::vec3(1.0f)
    ) const
    {
        shader.Use();

        shader.SetMat4("Model", model);
        shader.SetMat4("View", view);
        shader.SetMat4("Projection", projection);
        shader.SetVec3("ViewPos", viewPos);
        shader.SetVec3("uColor", color);

        mesh.Draw();
    }
};