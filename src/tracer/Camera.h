//
// Created by Ian on 3-1-2024.
//

#ifndef ADVANCEDGRAPHICS_CAMERA_H
#define ADVANCEDGRAPHICS_CAMERA_H

#include "Ray.h"
#include "../utils/Math.h"
#include "../core/Window.h"

class Camera {
    vec3f Up = vec3f(0.0f, 1.0f, 0.0f);

    vec3f m_TopLeft;
    vec3f m_TopRight;
    vec3f m_BottomLeft;

    float m_MoveSensitivity = 0.0023;
    float m_RotationSensitivity = 0.0013;
    float m_AspectRatio;
    float m_Fov = 40.f;

    WindowFrameSize m_Film;

    void UpdateSupportVectors();
public:
    vec3f m_Position;
    vec3f m_Target;

    Camera() = default;
    Camera(WindowFrameSize);
    Camera(WindowFrameSize, vec3f, vec3f);

    void ChangeAspectRation(WindowFrameSize&);

    void MoveForward(float deltaTime);
    void MoveRight(float deltaTime);
    void MoveBackward(float deltaTime);
    void MoveLeft(float deltaTime);
    void MoveUp(float deltaTime);
    void MoveDown(float deltaTime);

    void ChangeOrientation(float, float);

    Ray GetPrimaryRay(int width, int height);
};


#endif //ADVANCEDGRAPHICS_CAMERA_H
