//
// Created by Ian on 3-1-2024.
//

#ifndef ADVANCEDGRAPHICS_CAMERA_H
#define ADVANCEDGRAPHICS_CAMERA_H

#include "Ray.h"
#include "../utils/Math.h"
#include "../core/Window.h"

class Camera {
    vec3f m_Position;
    vec3f m_Target;

    vec3f m_Up = vec3f(0.0f, 1.0f, 0.0f);
    vec3f m_Left;
    vec3f m_Top;

    float m_MoveSensitivity = 0.023;
    float m_RotationSensitivity = 0.083;

    float m_Fov = 75.f;
    float m_Distance = 1.0f;
    float m_AspectRatio = 1.0f;

    float m_Yaw = 0.00f;
    float m_Pitch = 0.00f;

    WindowFrameSize m_Film;

    void UpdateSupportVectors();
    [[nodiscard]] vec3f CalculateViewDirection() const;
public:
    Camera() = default;
    Camera(WindowFrameSize);
    Camera(WindowFrameSize, vec3f, vec3f);

    const float GetFieldOfView() const;

    void MoveForward(float deltaTime);
    void MoveRight(float deltaTime);
    void MoveBackward(float deltaTime);
    void MoveLeft(float deltaTime);
    void MoveUp(float deltaTime);
    void MoveDown(float deltaTime);

    void ChangeOrientation(float, float, float);
    void ChangeAspectRation(WindowFrameSize&);
    void ChangeFov(float fov);

    Ray GetPrimaryRay(float width, float height);
};


#endif //ADVANCEDGRAPHICS_CAMERA_H
