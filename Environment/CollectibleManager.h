
#include <vector>
#include <string>
#include <iostream>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Model.h"
#include "Core/Renderer.h"
#include "Shaders/Shader.h"

struct Collectible
{
    glm::vec3 position;
    float scale = 0.45f;
    float rotationY = 0.0f;
    int points = 10;
    bool collected = false;
};

class CollectibleManager
{
public:
    CollectibleManager(const std::string& modelPath)
        : coinModel(modelPath)
    {
        GenerateCoins();
    }

    void GenerateCoins()
    {
        items.clear();

        // 1. Cztery monety zawieszone dokładnie w środkach 4 kółek pod sufitem
        AddCoin({ 6.3f, 3.8f, -1.2f }, 0.45f, 10);
        AddCoin({ 6.3f, 4.4f,  1.2f }, 0.45f, 10);
        AddCoin({ 6.3f, 5.0f,  3.6f }, 0.45f, 10);
        AddCoin({ 6.3f, 5.6f,  6.0f }, 0.45f, 20); // ostatnie najwyższe kółko za 20 pkt

        // 2. Dodatkowe punkty do zebrania w ciekawych miejscach pokoju
        AddCoin({ 0.0f, 2.3f, -6.8f }, 0.45f, 10); // nad stołem
        AddCoin({ -3.8f, 1.8f,  1.5f }, 0.45f, 10); // nad stolikiem kawowym
        AddCoin({ 6.25f, 2.1f, -4.5f }, 0.45f, 25); // wewnątrz tunelu budki na drapaku
        AddCoin({ 0.0f, 4.8f,  0.0f }, 0.45f, 15); // pod żyrandolem
    }

    void Update(float deltaTime, const glm::vec3& planePosition, int& score)
    {
        elapsedTime += deltaTime;

        const float rotationSpeed = 120.0f; // stopnie na sekundę
        const float collectRadius = 0.85f;  // promień zbierania przez samolot

        for (auto& item : items)
        {
            if (item.collected)
                continue;

            // Obrót monety wokół własnej osi Y
            item.rotationY += rotationSpeed * deltaTime;
            if (item.rotationY >= 360.0f)
                item.rotationY -= 360.0f;

            // Sprawdzenie czy samolot wleciał w monetę
            float distance = glm::distance(planePosition, item.position);
            if (distance <= collectRadius)
            {
                item.collected = true;
                score += item.points;
                std::cout << "Zebrano punkt! (+" << item.points << ") | Wynik: " << score << '\n';
            }
        }
    }

    void Draw(
        const Renderer& renderer,
        const Shader& shader,
        const glm::mat4& view,
        const glm::mat4& projection,
        const glm::vec3& viewPos
    ) const
    {


        for (const auto& item : items)
        {
            if (item.collected)
                continue;

            float bobOffset = std::sin(elapsedTime * 3.0f + item.position.x) * 0.08f;

            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, item.position + glm::vec3(0.0f, bobOffset, 0.0f));
            model = glm::rotate(model, glm::radians(item.rotationY), glm::vec3(0.0f, 1.0f, 0.0f));

            // Jeśli moneta z pliku .fbx leży płasko zamiast stać pionowo, odkomentuj poniższą linię:
            // model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

            model = glm::scale(model, glm::vec3(item.scale));

            // glm::vec3(1.0f) zachowuje oryginalny kolor z pliku .fbx
            // (jeśli plik .fbx nie ma zapisanego koloru, wpisz tu np. glm::vec3(1.0f, 0.84f, 0.0f))
            coinModel.Draw(renderer, shader, model, view, projection, viewPos, glm::vec3(1.0f));
        }
    }

    int GetRemainingCount() const
    {
        int count = 0;
        for (const auto& item : items)
            if (!item.collected) count++;
        return count;
    }

private:
    Model coinModel;
    float elapsedTime = 0.0f;
    std::vector<Collectible> items;

    void AddCoin(glm::vec3 pos, float scale, int points)
    {
        items.push_back({ pos, scale, 0.0f, points, false });
    }
};