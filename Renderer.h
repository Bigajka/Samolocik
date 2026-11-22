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
        const glm::mat4& projection
    ) const
    {
        shader.Use();

        shader.SetMat4("Model", model);
        shader.SetMat4("View", view);
        shader.SetMat4("Projection", projection);

        mesh.Draw();
    }
};