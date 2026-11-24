#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <string>

#include "Shaders/Shader.h"
#include "Core/Renderer.h"
#include "Core/Camera.h"
#include "Core/TextRenderer.h"
#include "Environment/Room.h"
#include "Environment/CollectibleManager.h"
#include "Environment/Model.h"

constexpr unsigned int WINDOW_WIDTH = 1280;
constexpr unsigned int WINDOW_HEIGHT = 720;

int currentWidth = WINDOW_WIDTH;
int currentHeight = WINDOW_HEIGHT;

Camera camera(glm::vec3(0.0f, 2.5f, 4.0f));
glm::mat4 projection = glm::mat4(1.0f);

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
        projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);
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

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void ProcessInput(GLFWwindow* window, float deltaTime)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (isWin) return; // Zatrzymanie sterowania samolotem po wygranej

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) camera.ProcessKeyboard(CameraMovement::FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) camera.ProcessKeyboard(CameraMovement::BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) camera.ProcessKeyboard(CameraMovement::LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) camera.ProcessKeyboard(CameraMovement::RIGHT, deltaTime);
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

    projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);

    glEnable(GL_DEPTH_TEST);

    Renderer renderer;
    TextRenderer textRenderer;
    Shader shader("Shaders/basic.vert", "Shaders/basic.frag");
    Room room;

    Model plane("Model/samolot.fbx");
    Model cat("Model/kot.fbx");
    CollectibleManager collectibles("Model/moneta.fbx");
    int score = 0;

    float lastFrame = (float)glfwGetTime();
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = (float)glfwGetTime();
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        ProcessInput(window, deltaTime);

        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        room.Draw(renderer, shader, camera.GetViewMatrix(), projection, camera.Position);

        glm::mat4 catModel = glm::mat4(1.0f);
        catModel = glm::translate(catModel, glm::vec3(3.5f, 5.95f, 3.5f));
        catModel = glm::rotate(catModel, glm::radians(-40.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        catModel = glm::scale(catModel, glm::vec3(0.85f));
        cat.Draw(renderer, shader, catModel, camera.GetViewMatrix(), projection, camera.Position, glm::vec3(0.85f, 0.45f, 0.18f));

        // Samolot
        glm::vec3 planePos = camera.Position + camera.Front * 1.5f - camera.Up * 0.4f;

        glm::mat4 planeModel = glm::mat4(1.0f);
        planeModel = glm::translate(planeModel, planePos);
        planeModel = glm::rotate(planeModel, glm::radians(-camera.Yaw + 90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        planeModel = glm::rotate(planeModel, glm::radians(camera.Pitch), glm::vec3(1.0f, 0.0f, 0.0f));
        planeModel = glm::scale(planeModel, glm::vec3(0.8f));

        if (!isWin)
        {
            collectibles.Update(deltaTime, planePos, score);
            if (collectibles.GetRemainingCount() == 0)
            {
                isWin = true;
            }
        }

        collectibles.Draw(renderer, shader, camera.GetViewMatrix(), projection, camera.Position);
        plane.Draw(renderer, shader, planeModel, camera.GetViewMatrix(), projection, camera.Position, glm::vec3(0.9f, 0.2f, 0.2f));

        textRenderer.Begin(currentWidth, currentHeight);

        // Pasek wyników w lewym górnym rogu
        std::string scoreText = "SCORE: " + std::to_string(score);
        std::string coinsText = "COINS LEFT: " + std::to_string(collectibles.GetRemainingCount());

        textRenderer.DrawRect(15.0f, 15.0f, 320.0f, 82.0f, glm::vec4(0.0f, 0.0f, 0.0f, 0.55f));
        textRenderer.DrawText(scoreText, 28.0f, 26.0f, 3.5f, glm::vec3(1.0f, 0.85f, 0.15f));
        textRenderer.DrawText(coinsText, 28.0f, 62.0f, 3.0f, glm::vec3(0.95f, 0.95f, 0.95f));

        if (isWin)
        {
            // przyciemnienie całego ekranu
            textRenderer.DrawRect(0.0f, 0.0f, static_cast<float>(currentWidth), static_cast<float>(currentHeight), glm::vec4(0.0f, 0.0f, 0.0f, 0.6f));

            std::string winText = "WIN!";
            float winScale = 24.0f;
            float winX = (currentWidth - textRenderer.GetTextWidth(winText, winScale)) * 0.5f;
            float winY = (currentHeight - textRenderer.GetTextHeight(winScale)) * 0.5f - 30.0f;

            // złoty napis WIN!
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