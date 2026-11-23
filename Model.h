#pragma once
#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include <string>
#include <iostream>

#include "Environment/Mesh.h"
#include "Core/Renderer.h"

class Model
{
public:
    Model(const std::string& path)
    {
        LoadModel(path);
    }

    void Draw(
        const Renderer& renderer,
        const Shader& shader,
        const glm::mat4& modelMatrix,
        const glm::mat4& view,
        const glm::mat4& projection,
        const glm::vec3& viewPos,
        const glm::vec3& color = glm::vec3(1.0f)
    ) const
    {
        for (const auto& mesh : meshes)
        {
            renderer.Draw(*mesh, shader, modelMatrix, view, projection, viewPos, color);
        }
    }

private:
    std::vector<std::unique_ptr<Mesh>> meshes;

    void LoadModel(const std::string& path)
    {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(
            path,
            aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs
        );

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            std::cerr << "ASSIMP ERROR: " << importer.GetErrorString() << '\n';
            return;
        }

        ProcessNode(scene->mRootNode, scene);
        std::cout << "Zaladowano model: " << path << " (siatek: " << meshes.size() << ")\n";
    }

    void ProcessNode(aiNode* node, const aiScene* scene)
    {
        for (unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            meshes.push_back(ProcessMesh(mesh, scene));
        }

        for (unsigned int i = 0; i < node->mNumChildren; i++)
        {
            ProcessNode(node->mChildren[i], scene);
        }
    }

    std::unique_ptr<Mesh> ProcessMesh(aiMesh* mesh, const aiScene* scene)
    {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;

        // Domyślny kolor materiału z pliku FBX (jeśli istnieje)
        glm::vec3 matColor(1.0f);
        if (mesh->mMaterialIndex >= 0)
        {
            aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
            aiColor3D diffuse(1.0f, 1.0f, 1.0f);
            if (material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse) == AI_SUCCESS)
            {
                matColor = glm::vec3(diffuse.r, diffuse.g, diffuse.b);
            }
        }

        for (unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            Vertex vertex;
            vertex.Position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);

            if (mesh->HasNormals())
                vertex.Normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
            else
                vertex.Normal = glm::vec3(0.0f, 1.0f, 0.0f);

            vertex.Color = matColor;
            vertices.push_back(vertex);
        }

        for (unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++)
            {
                indices.push_back(face.mIndices[j]);
            }
        }

        return std::make_unique<Mesh>(vertices, indices);
    }
};