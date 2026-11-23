#pragma once

#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include "Environment/Mesh.h"
#include "SceneObject.h"
#include "Core/Renderer.h"

class Room
{
public:
    Room()
    {
        cubeMesh = CreateUnitCube();
        BuildRoom();
    }

    void Draw(
        const Renderer& renderer,
        const Shader& shader,
        const glm::mat4& view,
        const glm::mat4& projection,
        const glm::vec3& viewPos
    ) const
    {
        for (const auto& obj : objects)
        {
            renderer.Draw(
                *obj.mesh,
                shader,
                obj.GetModelMatrix(),
                view,
                projection,
                viewPos,
                obj.color
            );
        }
    }

    // Przyda się później do detekcji kolizji (AABB)
    const std::vector<SceneObject>& GetObjects() const { return objects; }

private:
    std::unique_ptr<Mesh> cubeMesh;
    std::vector<SceneObject> objects;

    void AddBox(glm::vec3 pos, glm::vec3 scale, glm::vec3 color, glm::vec3 rot = glm::vec3(0.0f))
    {
        objects.push_back({ cubeMesh.get(), pos, rot, scale, color });
    }

    void BuildRoom()
    {
        AddWallsAndFloor();

        // Rozmieszczenie mebli w pokoju (X: -8..8, Y: 0..7, Z: -8..8)
        AddDesk(glm::vec3(0.0f, 0.0f, -6.8f));        // Biurko pod tylną ścianą
        AddChair(glm::vec3(0.0f, 0.0f, -5.2f));       // Krzesło przy biurku
        AddSofa(glm::vec3(-6.6f, 0.0f, 1.5f));        // Kanapa pod lewą ścianą
        AddCoffeeTable(glm::vec3(-3.8f, 0.0f, 1.5f)); // Stolik kawowy przed kanapą
        AddChair(glm::vec3(4.5f, 0.0f, 3.0f));        // Drugie krzesło w rogu
    }

    void AddWallsAndFloor()
    {
        // Podłoga (Y = 0 to powierzchnia podłogi)
        AddBox({ 0.0f, -0.1f, 0.0f }, { 16.0f, 0.2f, 16.0f }, { 0.45f, 0.32f, 0.22f });
        // Sufit (wysokość 7.0)
        AddBox({ 0.0f, 7.1f, 0.0f }, { 16.0f, 0.2f, 16.0f }, { 0.90f, 0.90f, 0.90f });
        // Ściany
        AddBox({ 0.0f, 3.5f, -8.1f }, { 16.0f, 7.0f, 0.2f }, { 0.82f, 0.80f, 0.75f }); // tył
        AddBox({ 0.0f, 3.5f,  8.1f }, { 16.0f, 7.0f, 0.2f }, { 0.82f, 0.80f, 0.75f }); // przód
        AddBox({ -8.1f, 3.5f, 0.0f }, { 0.2f, 7.0f, 16.0f }, { 0.76f, 0.74f, 0.70f }); // lewa
        AddBox({ 8.1f, 3.5f, 0.0f }, { 0.2f, 7.0f, 16.0f }, { 0.76f, 0.74f, 0.70f }); // prawa
    }

    void AddDesk(glm::vec3 base)
    {
        glm::vec3 wood = { 0.55f, 0.35f, 0.18f };
        // Blat
        AddBox(base + glm::vec3(0.0f, 1.4f, 0.0f), { 3.2f, 0.15f, 1.5f }, wood);
        // 4 nogi
        AddBox(base + glm::vec3(-1.45f, 0.7f, -0.6f), { 0.15f, 1.4f, 0.15f }, wood);
        AddBox(base + glm::vec3(1.45f, 0.7f, -0.6f), { 0.15f, 1.4f, 0.15f }, wood);
        AddBox(base + glm::vec3(-1.45f, 0.7f, 0.6f), { 0.15f, 1.4f, 0.15f }, wood);
        AddBox(base + glm::vec3(1.45f, 0.7f, 0.6f), { 0.15f, 1.4f, 0.15f }, wood);
    }

    void AddCoffeeTable(glm::vec3 base)
    {
        glm::vec3 wood = { 0.40f, 0.25f, 0.15f };
        // Blat
        AddBox(base + glm::vec3(0.0f, 0.8f, 0.0f), { 1.4f, 0.12f, 2.4f }, wood);
        // 4 nogi
        AddBox(base + glm::vec3(-0.58f, 0.4f, -1.05f), { 0.12f, 0.8f, 0.12f }, wood);
        AddBox(base + glm::vec3(0.58f, 0.4f, -1.05f), { 0.12f, 0.8f, 0.12f }, wood);
        AddBox(base + glm::vec3(-0.58f, 0.4f, 1.05f), { 0.12f, 0.8f, 0.12f }, wood);
        AddBox(base + glm::vec3(0.58f, 0.4f, 1.05f), { 0.12f, 0.8f, 0.12f }, wood);
    }

    void AddChair(glm::vec3 base)
    {
        glm::vec3 wood = { 0.60f, 0.40f, 0.22f };
        // Siedzisko
        AddBox(base + glm::vec3(0.0f, 0.85f, 0.0f), { 0.9f, 0.1f, 0.9f }, wood);
        // Oparcie
        AddBox(base + glm::vec3(0.0f, 1.45f, 0.4f), { 0.9f, 1.1f, 0.1f }, wood);
        // 4 nogi
        AddBox(base + glm::vec3(-0.38f, 0.42f, -0.38f), { 0.1f, 0.85f, 0.1f }, wood);
        AddBox(base + glm::vec3(0.38f, 0.42f, -0.38f), { 0.1f, 0.85f, 0.1f }, wood);
        AddBox(base + glm::vec3(-0.38f, 0.42f, 0.38f), { 0.1f, 0.85f, 0.1f }, wood);
        AddBox(base + glm::vec3(0.38f, 0.42f, 0.38f), { 0.1f, 0.85f, 0.1f }, wood);
    }

    void AddSofa(glm::vec3 base)
    {
        glm::vec3 fabric = { 0.22f, 0.32f, 0.48f }; // granatowa tapicerka
        // Siedzisko (skierowane w stronę środka pokoju, wzdłuż osi Z)
        AddBox(base + glm::vec3(0.0f, 0.45f, 0.0f), { 1.6f, 0.9f, 4.2f }, fabric);
        // Oparcie (przy ścianie, po lewej stronie kanapy)
        AddBox(base + glm::vec3(-0.65f, 1.3f, 0.0f), { 0.3f, 1.0f, 4.2f }, fabric);
        // Podłokietniki
        AddBox(base + glm::vec3(0.0f, 1.05f, -1.95f), { 1.6f, 0.5f, 0.3f }, fabric);
        AddBox(base + glm::vec3(0.0f, 1.05f, 1.95f), { 1.6f, 0.5f, 0.3f }, fabric);
    }

    static std::unique_ptr<Mesh> CreateUnitCube()
    {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        glm::vec3 white(1.0f);
        float s = 0.5f;

        auto addFace = [&](glm::vec3 n, glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 d)
            {
                unsigned int start = static_cast<unsigned int>(vertices.size());
                vertices.push_back({ a, n, white });
                vertices.push_back({ b, n, white });
                vertices.push_back({ c, n, white });
                vertices.push_back({ d, n, white });
                indices.insert(indices.end(), { start, start + 1, start + 2, start, start + 2, start + 3 });
            };

        addFace({ 0,  0,  1 }, { -s,-s, s }, { s,-s, s }, { s, s, s }, { -s, s, s }); // front
        addFace({ 0,  0, -1 }, { -s,-s,-s }, { -s, s,-s }, { s, s,-s }, { s,-s,-s }); // back
        addFace({ 1,  0,  0 }, { s,-s,-s }, { s, s,-s }, { s, s, s }, { s,-s, s }); // right
        addFace({ -1,  0,  0 }, { -s,-s,-s }, { -s,-s, s }, { -s, s, s }, { -s, s,-s }); // left
        addFace({ 0,  1,  0 }, { -s, s,-s }, { -s, s, s }, { s, s, s }, { s, s,-s }); // up
        addFace({ 0, -1,  0 }, { -s,-s,-s }, { s,-s,-s }, { s,-s, s }, { -s,-s, s }); // down

        return std::make_unique<Mesh>(vertices, indices);
    }
};