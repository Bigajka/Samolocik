#pragma once

#include <vector>
#include <memory>
#include <cmath>
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
        AddChair(glm::vec3(0.0f, 0.0f, -5.4f), 0.0f); 
        AddChair(glm::vec3(-2.55f, 0.0f, -6.8f), -90.0f); 
        AddChair(glm::vec3(2.55f, 0.0f, -6.8f), 90.0f);
        AddSofa(glm::vec3(-6.6f, 0.0f, 1.5f));        // Kanapa pod lewą ścianą
        AddCoffeeTable(glm::vec3(-3.8f, 0.0f, 1.5f)); // Stolik kawowy przed kanapą
        AddChandelier(glm::vec3(0.0f, 7.0f, 0.0f));   // Żyrandol na środku sufitu
        AddPlant(glm::vec3(-6.6f, 0.0f, -1.3f));      // Roślinka przy kanapie
        AddStandingLamp(glm::vec3(-6.6f, 0.0f, 4.3f));// Lampa przy kanapie
        AddCatTree(glm::vec3(4.5f, 0.0f, 4.5f));      // Drapak dla kota (pole manewrowe)
        AddHangingRing(glm::vec3(6.3f, 3.8f, -6.0f), 0.85f, 0.0f); // Kółka pod sufitem
        AddHangingRing(glm::vec3(6.3f, 4.4f, -4.0f), 0.85f, 0.0f);
        AddHangingRing(glm::vec3(6.3f, 5.0f, -2.0f), 0.85f, 0.0f);
        AddHangingRing(glm::vec3(6.3f, 5.6f, -0.0f), 0.85f, 0.0f);                           
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

    void AddChair(glm::vec3 base, float rotationY = 0.0f)
    {
        glm::vec3 wood = { 0.60f, 0.40f, 0.22f };
        glm::mat4 rotMat = glm::rotate(glm::mat4(1.0f), glm::radians(rotationY), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::vec3 rotVec(0.0f, rotationY, 0.0f);

        auto addPart = [&](glm::vec3 localOffset, glm::vec3 scale)
            {
                glm::vec3 rotatedOffset = glm::vec3(rotMat * glm::vec4(localOffset, 1.0f));
                AddBox(base + rotatedOffset, scale, wood, rotVec);
            };

        // Siedzisko
        addPart({ 0.0f, 0.85f, 0.0f }, { 0.9f, 0.1f, 0.9f });
        // Oparcie
        addPart({ 0.0f, 1.45f, 0.4f }, { 0.9f, 1.1f, 0.1f });
        // 4 nogi
        addPart({ -0.38f, 0.42f, -0.38f }, { 0.1f, 0.85f, 0.1f });
        addPart({ 0.38f, 0.42f, -0.38f }, { 0.1f, 0.85f, 0.1f });
        addPart({ -0.38f, 0.42f,  0.38f }, { 0.1f, 0.85f, 0.1f });
        addPart({ 0.38f, 0.42f,  0.38f }, { 0.1f, 0.85f, 0.1f });
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

    void AddChandelier(glm::vec3 ceilingPos)
    {
        glm::vec3 metal = { 0.75f, 0.60f, 0.25f }; 
        glm::vec3 bulb = { 1.00f, 0.95f, 0.75f };

        AddBox(ceilingPos + glm::vec3(0.0f, -0.05f, 0.0f), { 0.5f, 0.1f, 0.5f }, metal);
        AddBox(ceilingPos + glm::vec3(0.0f, -0.60f, 0.0f), { 0.08f, 1.2f, 0.08f }, metal);

        AddBox(ceilingPos + glm::vec3(0.0f, -1.15f, 0.0f), { 1.6f, 0.08f, 0.08f }, metal);
        AddBox(ceilingPos + glm::vec3(0.0f, -1.15f, 0.0f), { 0.08f, 0.08f, 1.6f }, metal);

        AddBox(ceilingPos + glm::vec3(0.75f, -0.98f, 0.0f), { 0.22f, 0.28f, 0.22f }, bulb);
        AddBox(ceilingPos + glm::vec3(-0.75f, -0.98f, 0.0f), { 0.22f, 0.28f, 0.22f }, bulb);
        AddBox(ceilingPos + glm::vec3(0.0f, -0.98f, 0.75f), { 0.22f, 0.28f, 0.22f }, bulb);
        AddBox(ceilingPos + glm::vec3(0.0f, -0.98f, -0.75f), { 0.22f, 0.28f, 0.22f }, bulb);
    }

    void AddPlant(glm::vec3 base)
    {
        glm::vec3 pot = { 0.65f, 0.30f, 0.18f }; 
        glm::vec3 soil = { 0.20f, 0.12f, 0.08f };
        glm::vec3 stem = { 0.25f, 0.35f, 0.15f };
        glm::vec3 leaf1 = { 0.18f, 0.52f, 0.20f }; 
        glm::vec3 leaf2 = { 0.24f, 0.65f, 0.26f }; 

        AddBox(base + glm::vec3(0.0f, 0.30f, 0.0f), { 0.55f, 0.60f, 0.55f }, pot);
        AddBox(base + glm::vec3(0.0f, 0.61f, 0.0f), { 0.48f, 0.04f, 0.48f }, soil);

        AddBox(base + glm::vec3(0.0f, 0.95f, 0.0f), { 0.08f, 0.70f, 0.08f }, stem);
        AddBox(base + glm::vec3(0.0f, 1.30f, 0.0f), { 0.70f, 0.45f, 0.70f }, leaf1);
        AddBox(base + glm::vec3(0.0f, 1.65f, 0.0f), { 0.50f, 0.40f, 0.50f }, leaf2, { 0.0f, 45.0f, 0.0f });
        AddBox(base + glm::vec3(0.0f, 1.95f, 0.0f), { 0.28f, 0.30f, 0.28f }, leaf1);
    }

    void AddStandingLamp(glm::vec3 base)
    {
        glm::vec3 metal = { 0.20f, 0.20f, 0.22f };
        glm::vec3 shade = { 0.95f, 0.90f, 0.72f };

        AddBox(base + glm::vec3(0.0f, 0.05f, 0.0f), { 0.50f, 0.10f, 0.50f }, metal);
        AddBox(base + glm::vec3(0.0f, 1.15f, 0.0f), { 0.07f, 2.20f, 0.07f }, metal);
        AddBox(base + glm::vec3(0.0f, 2.35f, 0.0f), { 0.55f, 0.50f, 0.55f }, shade);
    }

    void AddCatTree(glm::vec3 base)
    {
        glm::vec3 plushLight = { 0.86f, 0.82f, 0.75f }; 
        glm::vec3 plushDark = { 0.42f, 0.40f, 0.44f }; 
        glm::vec3 sisal = { 0.76f, 0.62f, 0.42f };
        glm::vec3 toyColor = { 0.90f, 0.32f, 0.24f };

        AddBox(base + glm::vec3(0.0f, 0.10f, 0.0f), { 3.4f, 0.20f, 3.4f }, plushDark);

        AddBox(base + glm::vec3(-1.2f, 1.50f, 1.2f), { 0.28f, 2.60f, 0.28f }, sisal); 
        AddBox(base + glm::vec3(1.2f, 2.20f, 1.2f), { 0.28f, 4.00f, 0.28f }, sisal); 
        AddBox(base + glm::vec3(1.2f, 2.20f, -1.2f), { 0.28f, 4.00f, 0.28f }, sisal);
        AddBox(base + glm::vec3(-1.2f, 2.80f, -1.2f), { 0.28f, 5.20f, 0.28f }, sisal);

        AddBox(base + glm::vec3(0.75f, 1.45f, 0.0f), { 1.60f, 0.12f, 2.20f }, plushLight); 
        AddBox(base + glm::vec3(0.75f, 2.75f, 0.0f), { 1.70f, 0.14f, 2.30f }, plushLight); 
        AddBox(base + glm::vec3(0.02f, 2.10f, 0.0f), { 0.14f, 1.18f, 2.20f }, plushDark);
        AddBox(base + glm::vec3(1.48f, 2.10f, 0.0f), { 0.14f, 1.18f, 2.20f }, plushDark); 

        AddBox(base + glm::vec3(-0.95f, 2.85f, 0.0f), { 1.30f, 0.15f, 2.80f }, plushLight);

        AddBox(base + glm::vec3(0.0f, 4.25f, -1.0f), { 3.00f, 0.15f, 1.10f }, plushLight); 
        AddBox(base + glm::vec3(1.05f, 4.25f, 0.4f), { 1.10f, 0.15f, 1.80f }, plushLight); 

        AddBox(base + glm::vec3(-0.30f, 3.85f, -0.60f), { 0.04f, 0.65f, 0.04f }, sisal);
        AddBox(base + glm::vec3(-0.30f, 3.45f, -0.60f), { 0.24f, 0.24f, 0.24f }, toyColor);

        AddBox(base + glm::vec3(-1.0f, 5.45f, -1.0f), { 1.30f, 0.14f, 1.30f }, plushLight);
        AddBox(base + glm::vec3(-1.0f, 5.62f, -1.60f), { 1.30f, 0.20f, 0.10f }, plushDark);
        AddBox(base + glm::vec3(-1.0f, 5.62f, -0.40f), { 1.30f, 0.20f, 0.10f }, plushDark);
        AddBox(base + glm::vec3(-1.60f, 5.62f, -1.0f), { 0.10f, 0.20f, 1.10f }, plushDark);
        AddBox(base + glm::vec3(-0.40f, 5.62f, -1.0f), { 0.10f, 0.20f, 1.10f }, plushDark);
    }

    void AddCeilingRings()
    {
        // Linia 4 kółek pod sufitem: każde kolejne przesunięte w prawo (+X) i o 0.6 wyżej (+Y)
        // Kąt yaw = 90.0f ustawia otwór obręczy wzdłuż linii lotu (osi X)
        AddHangingRing(glm::vec3(-2.5f, 3.8f, -2.2f), 0.85f, 90.0f);
        AddHangingRing(glm::vec3(0.0f, 4.4f, -2.2f), 0.85f, 90.0f);
        AddHangingRing(glm::vec3(2.5f, 5.0f, -2.2f), 0.85f, 90.0f);
        AddHangingRing(glm::vec3(5.0f, 5.6f, -2.2f), 0.85f, 90.0f);
    }

    void AddHangingRing(glm::vec3 center, float radius, float yawDeg)
    {
        glm::vec3 ringColor = { 0.95f, 0.72f, 0.15f };
        glm::vec3 ropeColor = { 0.22f, 0.22f, 0.25f };

        const int segments = 20;
        const float thickness = 0.12f;
        const float segLength = 2.0f * radius * std::tan(glm::radians(180.0f / segments)) + 0.02f;

        float yawRad = glm::radians(yawDeg);
        float cosYaw = std::cos(yawRad);
        float sinYaw = std::sin(yawRad);

        for (int i = 0; i < segments; ++i)
        {
            float angleDeg = (360.0f / segments) * i;
            float angleRad = glm::radians(angleDeg);

            float localX = radius * std::cos(angleRad);
            float localY = radius * std::sin(angleRad);

            glm::vec3 worldOffset(
                localX * cosYaw,
                localY,
                -localX * sinYaw
            );

            AddBox(
                center + worldOffset,
                { segLength, thickness, thickness },
                ringColor,
                { 0.0f, yawDeg, angleDeg + 90.0f }
            );
        }

        float ceilingY = 7.0f;
        float ringTopY = center.y + radius;
        float ropeLength = ceilingY - ringTopY;

        if (ropeLength > 0.0f)
        {
            AddBox(
                { center.x, ringTopY + ropeLength * 0.5f, center.z },
                { 0.035f, ropeLength, 0.035f },
                ropeColor
            );
            AddBox(
                { center.x, ceilingY - 0.03f, center.z },
                { 0.18f, 0.06f, 0.18f },
                ropeColor
            );
        }
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