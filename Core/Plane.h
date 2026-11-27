#pragma once
#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include <cmath>

class Plane
{
public:
    glm::vec3 Position;
    glm::vec3 Front{ 0.0f, 0.0f, -1.0f };
    glm::vec3 Right{ 1.0f, 0.0f,  0.0f };
    glm::vec3 Up{ 0.0f, 1.0f,  0.0f };
    glm::vec3 Velocity{ 0.0f, 0.0f, -2.6f };

    // Kąty orientacji samolotu (w stopniach)
    float Yaw = -90.0f;
    float Pitch = 0.0f;
    float Roll = 0.0f;

    // Prędkości lotu
    float CruiseSpeed = 2.6f;   // bazowa prędkość przelotowa
    float BoostSpeed = 5.2f;    // przyspieszenie (Left Shift)
    float BrakeSpeed = 0.8f;    // zwolnienie (Spacja)
    float CurrentSpeed = 2.6f;

    // Czułość sterowania klawiaturą
    float PitchSpeed = 65.0f;   // szybkość wznoszenia / opadania (W / S)
    float TurnSpeed = 85.0f;    // szybkość skrętu lewo / prawo (A / D)
    float MaxBankAngle = 42.0f; // przechył na skrzydło podczas skrętu

    Plane(glm::vec3 startPos = glm::vec3(0.0f, 2.5f, 3.5f))
        : Position(startPos)
    {
        UpdateVectors();
        Velocity = Front * CruiseSpeed;
    }

    void Update(
        float deltaTime,
        bool pitchUp,
        bool pitchDown,
        bool turnLeft,
        bool turnRight,
        bool accelerate,
        bool decelerate
    )
    {
        // 1. Przepustnica: Shift = przyspieszenie, Spacja = zwolnienie
        float targetSpeed = CruiseSpeed;
        if (accelerate)      targetSpeed = BoostSpeed;
        else if (decelerate) targetSpeed = BrakeSpeed;

        CurrentSpeed = glm::mix(CurrentSpeed, targetSpeed, std::min(1.0f, deltaTime * 3.5f));

        // 2. Wznoszenie i opadanie (W / S)
        float pitchInput = 0.0f;
        if (pitchUp)   pitchInput += 1.0f;
        if (pitchDown) pitchInput -= 1.0f;

        if (pitchInput != 0.0f)
        {
            Pitch += pitchInput * PitchSpeed * deltaTime;
            Pitch = std::clamp(Pitch, -65.0f, 65.0f);
        }
        else
        {
            // Automatyczny powrót dziobu do poziomu po puszczeniu W lub S
            float autoLevelSpeed = 4.5f;
            Pitch = glm::mix(Pitch, 0.0f, std::min(1.0f, deltaTime * autoLevelSpeed));

            if (std::abs(Pitch) < 0.05f)
                Pitch = 0.0f;
        }

        Pitch += pitchInput * PitchSpeed * deltaTime;
        Pitch = std::clamp(Pitch, -65.0f, 65.0f);

        // 3. Skręcanie (A / D) i przechył na skrzydło (Roll)
        float turnInput = 0.0f;
        if (turnLeft)  turnInput -= 1.0f;
        if (turnRight) turnInput += 1.0f;

        Yaw += turnInput * TurnSpeed * deltaTime;

        // Skręt w lewo (turnInput = -1) przechyla na lewe skrzydło (+Roll), w prawo (-Roll)
        float targetRoll = -turnInput * MaxBankAngle;
        Roll = glm::mix(Roll, targetRoll, std::min(1.0f, deltaTime * 7.5f));

        // 4. Ruch samolotu zawsze do przodu wzdłuż wektora Front
        UpdateVectors();

        glm::vec3 desiredVelocity = Front * CurrentSpeed;
        Velocity = glm::mix(Velocity, desiredVelocity, std::min(1.0f, deltaTime * 8.0f));
        Position += Velocity * deltaTime;

        // 5. Zabezpieczenie przed wyleceniem poza ściany pokoju
        //Position.x = std::clamp(Position.x, -7.2f, 7.2f);
        //Position.y = std::clamp(Position.y, 0.45f, 6.45f);
        //Position.z = std::clamp(Position.z, -7.2f, 7.2f);
    }

    glm::mat4 GetModelMatrix() const
    {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, Position);

        model = glm::rotate(model, glm::radians(-Yaw + 90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, glm::radians(-Pitch), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(Roll), glm::vec3(0.0f, 0.0f, 1.0f));

        model = glm::scale(model, glm::vec3(0.8f));
        return model;
    }

    float GetSpeedFactor() const
    {
        return std::clamp((CurrentSpeed - BrakeSpeed) / (BoostSpeed - BrakeSpeed), 0.0f, 1.0f);
    }

private:
    void UpdateVectors()
    {
        glm::vec3 front;
        front.x = std::cos(glm::radians(Yaw)) * std::cos(glm::radians(Pitch));
        front.y = std::sin(glm::radians(Pitch));
        front.z = std::sin(glm::radians(Yaw)) * std::cos(glm::radians(Pitch));

        Front = glm::normalize(front);
        Right = glm::normalize(glm::cross(Front, glm::vec3(0.0f, 1.0f, 0.0f)));
        Up = glm::normalize(glm::cross(Right, Front));
    }
};