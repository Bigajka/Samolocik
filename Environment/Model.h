#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include <string>
#include <iostream>
#include <algorithm>
#include <cfloat>

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
            aiProcess_Triangulate |
            aiProcess_GenSmoothNormals |
            aiProcess_FlipUVs |
            aiProcess_PreTransformVertices
        );

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            std::cerr << "ASSIMP ERROR: " << importer.GetErrorString() << '\n';
            return;
        }

        glm::vec3 minB(FLT_MAX), maxB(-FLT_MAX);
        unsigned int totalVertices = 0;

        for (unsigned int i = 0; i < scene->mNumMeshes; i++)
        {
            aiMesh* m = scene->mMeshes[i];
            totalVertices += m->mNumVertices;
            for (unsigned int v = 0; v < m->mNumVertices; v++)
            {
                minB.x = std::min(minB.x, m->mVertices[v].x);
                minB.y = std::min(minB.y, m->mVertices[v].y);
                minB.z = std::min(minB.z, m->mVertices[v].z);

                maxB.x = std::max(maxB.x, m->mVertices[v].x);
                maxB.y = std::max(maxB.y, m->mVertices[v].y);
                maxB.z = std::max(maxB.z, m->mVertices[v].z);
            }
        }

        glm::vec3 center = (minB + maxB) * 0.5f;
        glm::vec3 size = maxB - minB;
        float maxExtent = std::max({ size.x, size.y, size.z });
        float normScale = (maxExtent > 0.0f) ? (1.0f / maxExtent) : 1.0f;

        std::cout << "Zaladowano model: " << path
            << " | siatek: " << scene->mNumMeshes
            << " | wierzcholkow: " << totalVertices << '\n';

        for (unsigned int i = 0; i < scene->mNumMeshes; i++)
        {
            meshes.push_back(ProcessMeshNormalized(scene->mMeshes[i], scene, center, normScale));
        }
    }

    std::unique_ptr<Mesh> ProcessMeshNormalized(
        aiMesh* mesh, const aiScene* scene, glm::vec3 center, float normScale)
    {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;

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
            glm::vec3 pos(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
            vertex.Position = (pos - center) * normScale;

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
                indices.push_back(face.mIndices[j]);
        }

        return std::make_unique<Mesh>(vertices, indices);
    }
};