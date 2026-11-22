#pragma once

#include <glad/glad.h>
#include <vector>
#include <cstddef>
#include "Vertex.h"

class Mesh
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    GLuint VAO = 0;
    GLuint VBO = 0;
    GLuint EBO = 0;

public:
    Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices): vertices(vertices), indices(indices)
    {
        SetupMesh();
    }

    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;

    ~Mesh()
    {
        if (EBO != 0)
            glDeleteBuffers(1, &EBO);

        if (VBO != 0)
            glDeleteBuffers(1, &VBO);

        if (VAO != 0)
            glDeleteVertexArrays(1, &VAO);
    }

    void Draw() const
    {
        glBindVertexArray(VAO);

        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, nullptr);

        glBindVertexArray(0);
    }

private:
    
    void SetupMesh()
    {
        // Generate OpenGL objects
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        // Bind VAO
        glBindVertexArray(VAO);

        // Vertex buffer
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        glBufferData(
            GL_ARRAY_BUFFER,
            vertices.size() * sizeof(Vertex),
            vertices.data(),
            GL_STATIC_DRAW
        );

        // Element buffer
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

        glBufferData(
            GL_ELEMENT_ARRAY_BUFFER,
            indices.size() * sizeof(unsigned int),
            indices.data(),
            GL_STATIC_DRAW
        );

        // Position attribute
        glVertexAttribPointer(
            0,
            3,
            GL_FLOAT,
            GL_FALSE,
            sizeof(Vertex),
            (void*)offsetof(Vertex, Position)
        );

        glEnableVertexAttribArray(0);

        // Normal
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
        glEnableVertexAttribArray(1);

        //color
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Color));
        glEnableVertexAttribArray(2);

        // Unbind VAO
        glBindVertexArray(0);
    }
};