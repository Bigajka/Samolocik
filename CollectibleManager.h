
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


        AddCoin({ 6.3f, 3.8f, -1.2f }, 0.45f, 10);
        AddCoin({ 6.3f, 4.4f,  1.2f }, 0.45f, 10);
        AddCoin({ 6.3f, 5.0f,  3.6f }, 0.45f, 10);
        AddCoin({ 6.3f, 5.6f,  6.0f }, 0.45f, 20); 

        AddCoin({ 0.0f, 2.3f, -6.8f }, 0.45f, 10); // nad stołem
        AddCoin({ -3.8f, 1.8f,  1.5f }, 0.45f, 10); // nad stolikiem kawowym
        AddCoin({ 6.25f, 2.1f, -4.5f }, 0.45f, 25); // wewnątrz tunelu budki na drapaku
        AddCoin({ 0.0f, 4.8f,  0.0f }, 0.45f, 15); // pod żyrandolem
    }

    void Update(float deltaTime, const glm::vec3& planePosition, int& score)
    {
        elapsedTime += deltaTime;

        const float rotationSpeed = 120.0f;
        const float collectRadius = 0.85f;

        for (auto& item : items)
        {
            if (item.collected)
                continue;

            item.rotationY += rotationSpeed * deltaTime;
            if (item.rotationY >= 360.0f)
                item.rotationY -= 360.0f;

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

            model = glm::scale(model, glm::vec3(item.scale));
          
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