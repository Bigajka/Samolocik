#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <string>
#include <algorithm>
#include <cmath>

#include "Shaders/Shader.h"
#include "Core/Renderer.h"
#include "Core/Camera.h"
#include "Core/Plane.h"
#include "Core/TextRenderer.h"
#include "Environment/Room.h"
#include "Environment/CollectibleManager.h"
#include "Environment/Model.h"

constexpr unsigned int WINDOW_WIDTH = 1280;
constexpr unsigned int WINDOW_HEIGHT = 720;

int currentWidth = WINDOW_WIDTH;
int currentHeight = WINDOW_HEIGHT;

Camera camera(glm::vec3(0.0f, 3.0f, 5.5f));
Plane playerPlane(glm::vec3(0.0f, 2.5f, 3.5f));
glm::mat4 projection = glm::mat4(1.0f);

// Kąty rozglądania się kamerą wokół samolotu
float camYawOffset = 0.0f;
float camPitchOffset = 15.0f;

float currentFov = 45.0f;
float lastX = WINDOW_WIDTH / 2.0f;
float lastY = WINDOW_HEIGHT / 2.0f;
bool firstMouse = true;
bool isWin = false;

void FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);

    if (width > 0 && height > 0)
    {
        currentWidth = width;
        currentHeight = height;
    }
}

void MouseCallback(GLFWwindow* window, double xposIn, double yposIn)
{
    if (isWin) return;

    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    const float lookSensitivity = 0.15f;
    camYawOffset += xoffset * lookSensitivity;
    camPitchOffset -= yoffset * lookSensitivity;
    camPitchOffset = std::clamp(camPitchOffset, -35.0f, 65.0f);
}

void UpdateOrbitCamera(float deltaTime)
{
    const float distance = 2.4f;

    float totalYaw = playerPlane.Yaw + camYawOffset;
    float yawRad = glm::radians(totalYaw);
    float pitchRad = glm::radians(camPitchOffset);

    glm::vec3 offset;
    offset.x = -std::cos(yawRad) * std::cos(pitchRad) * distance;
    offset.y = std::sin(pitchRad) * distance;
    offset.z = -std::sin(yawRad) * std::cos(pitchRad) * distance;

    glm::vec3 desiredCamPos = playerPlane.Position + offset;

    camera.Position = glm::mix(camera.Position, desiredCamPos, std::min(1.0f, deltaTime * 14.0f));

    glm::vec3 lookTarget = playerPlane.Position + glm::vec3(0.0f, 0.2f, 0.0f);
    camera.Front = glm::normalize(lookTarget - camera.Position);
    camera.Up = glm::vec3(0.0f, 1.0f, 0.0f);

    float targetFov = glm::mix(42.0f, 52.0f, playerPlane.GetSpeedFactor());
    currentFov = glm::mix(currentFov, targetFov, std::min(1.0f, deltaTime * 5.0f));

    if (currentWidth > 0 && currentHeight > 0)
    {
        projection = glm::perspective(
            glm::radians(currentFov),
            static_cast<float>(currentWidth) / static_cast<float>(currentHeight),
            0.1f,
            100.0f
        );
    }
}

int main()
{
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW\n";
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Samolocik 3D", nullptr, nullptr);
    if (!window)
    {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD\n";
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, MouseCallback);

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    currentWidth = width;
    currentHeight = height;

    glEnable(GL_DEPTH_TEST);

    Renderer renderer;
    TextRenderer textRenderer;
    Shader shader("Shaders/basic.vert", "Shaders/basic.frag");
    Room room;

    Model planeModelAsset("Model/samolot.fbx");
    Model cat("Model/kot.fbx");
    CollectibleManager collectibles("Model/moneta.fbx");
    int score = 0;

    float lastFrame = (float)glfwGetTime();
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = (float)glfwGetTime();
        float deltaTime = std::min(currentFrame - lastFrame, 0.05f);
        lastFrame = currentFrame;

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        if (!isWin)
        {
            bool pitchUp = glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS;
            bool pitchDown = glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS;
            bool turnLeft = glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS;
            bool turnRight = glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS;
            bool accelerate = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS;
            bool decelerate = glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;

            playerPlane.Update(deltaTime, pitchUp, pitchDown, turnLeft, turnRight, accelerate, decelerate);
            UpdateOrbitCamera(deltaTime);

            collectibles.Update(deltaTime, playerPlane.Position, score);
            if (collectibles.GetRemainingCount() == 0)
            {
                isWin = true;
            }
        }

        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Pokój i kot
        room.Draw(renderer, shader, camera.GetViewMatrix(), projection, camera.Position);

        glm::mat4 catModel = glm::mat4(1.0f);
        catModel = glm::translate(catModel, glm::vec3(3.5f, 5.95f, 3.5f));
        catModel = glm::rotate(catModel, glm::radians(-40.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        catModel = glm::scale(catModel, glm::vec3(0.85f));
        cat.Draw(renderer, shader, catModel, camera.GetViewMatrix(), projection, camera.Position, glm::vec3(0.85f, 0.45f, 0.18f));

        // Monety oraz samolot
        collectibles.Draw(renderer, shader, camera.GetViewMatrix(), projection, camera.Position);
        planeModelAsset.Draw(
            renderer,
            shader,
            playerPlane.GetModelMatrix(),
            camera.GetViewMatrix(),
            projection,
            camera.Position,
            glm::vec3(0.9f, 0.2f, 0.2f)
        );

        // Interfejs 2D
        textRenderer.Begin(currentWidth, currentHeight);

        std::string scoreText = "SCORE: " + std::to_string(score);
        std::string coinsText = "COINS LEFT: " + std::to_string(collectibles.GetRemainingCount());

        textRenderer.DrawRect(15.0f, 15.0f, 320.0f, 82.0f, glm::vec4(0.0f, 0.0f, 0.0f, 0.55f));
        textRenderer.DrawText(scoreText, 28.0f, 26.0f, 3.5f, glm::vec3(1.0f, 0.85f, 0.15f));
        textRenderer.DrawText(coinsText, 28.0f, 62.0f, 3.0f, glm::vec3(0.95f, 0.95f, 0.95f));

        // Panel sterowania
        if (!isWin)
        {
            const float boxWidth = 200.0f;
            const float boxHeight = 110.0f;
            const float boxX = 15.0f;
            const float boxY = static_cast<float>(currentHeight) - boxHeight - 15.0f;
            const float textX = boxX + 14.0f;
            const float scale = 2.2f;

            textRenderer.DrawRect(boxX, boxY, boxWidth, boxHeight, glm::vec4(0.0f, 0.0f, 0.0f, 0.55f));

            textRenderer.DrawText("WASD: STEER", textX, boxY + 14.0f, scale, glm::vec3(0.95f, 0.95f, 0.95f));
            textRenderer.DrawText("SHIFT: BOOST", textX, boxY + 36.0f, scale, glm::vec3(0.95f, 0.95f, 0.95f));
            textRenderer.DrawText("SPACE: BRAKE", textX, boxY + 58.0f, scale, glm::vec3(0.95f, 0.95f, 0.95f));
            textRenderer.DrawText("MOUSE: LOOK", textX, boxY + 80.0f, scale, glm::vec3(0.95f, 0.95f, 0.95f));
        }

        if (isWin)
        {
            textRenderer.DrawRect(0.0f, 0.0f, static_cast<float>(currentWidth), static_cast<float>(currentHeight), glm::vec4(0.0f, 0.0f, 0.0f, 0.6f));

            std::string winText = "WIN!";
            float winScale = 24.0f;
            float winX = (currentWidth - textRenderer.GetTextWidth(winText, winScale)) * 0.5f;
            float winY = (currentHeight - textRenderer.GetTextHeight(winScale)) * 0.5f - 30.0f;

            textRenderer.DrawText(winText, winX + 8.0f, winY + 8.0f, winScale, glm::vec3(0.15f, 0.1f, 0.0f));
            textRenderer.DrawText(winText, winX, winY, winScale, glm::vec3(1.0f, 0.84f, 0.1f));

            std::string finalScore = "FINAL SCORE: " + std::to_string(score);
            float subScale = 4.5f;
            float subX = (currentWidth - textRenderer.GetTextWidth(finalScore, subScale)) * 0.5f;
            textRenderer.DrawText(finalScore, subX, winY + textRenderer.GetTextHeight(winScale) + 35.0f, subScale, glm::vec3(1.0f, 1.0f, 1.0f));
        }

        textRenderer.End();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}