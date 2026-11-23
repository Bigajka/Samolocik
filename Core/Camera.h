#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum class CameraMovement { FORWARD, BACKWARD, LEFT, RIGHT };

class Camera
{
public:
    glm::vec3 Position;
    glm::vec3 Front = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);

    float Yaw = -90.0f;
    float Pitch = 0.0f;
    float Speed = 5.0f;
    float Sensitivity = 0.1f;

    Camera(glm::vec3 startPos = glm::vec3(0.0f, 2.0f, 5.0f)) : Position(startPos)
    {
        UpdateCameraVectors();
    }

    glm::mat4 GetViewMatrix() const
    {
        return glm::lookAt(Position, Position + Front, Up);
    }

    void ProcessKeyboard(CameraMovement direction, float deltaTime)
    {
        float velocity = Speed * deltaTime;
        if (direction == CameraMovement::FORWARD)  Position += Front * velocity;
        if (direction == CameraMovement::BACKWARD) Position -= Front * velocity;
        if (direction == CameraMovement::LEFT)     Position -= Right * velocity;
        if (direction == CameraMovement::RIGHT)    Position += Right * velocity;
    }

    void ProcessMouseMovement(float xoffset, float yoffset)
    {
        Yaw += xoffset * Sensitivity;
        Pitch += yoffset * Sensitivity;

        // Blokada obrotu góra-dół, żeby kamera nie zrobiła fikołka
        if (Pitch > 89.0f)  Pitch = 89.0f;
        if (Pitch < -89.0f) Pitch = -89.0f;

        UpdateCameraVectors();
    }

private:
    void UpdateCameraVectors()
    {
        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front = glm::normalize(front);
        Right = glm::normalize(glm::cross(Front, WorldUp));
        Up = glm::normalize(glm::cross(Right, Front));
    }
};