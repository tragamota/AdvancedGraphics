//
// Created by Ian on 3-1-2024.
//

#define _USE_MATH_DEFINES

#include "Camera.h"

#include <cmath>

Camera::Camera(WindowFrameSize frameSize) : m_Film(frameSize), m_Position(0, 0, 0), m_Target(0, 0, -1) {
    ChangeAspectRation(frameSize);
    UpdateSupportVectors();
}

Camera::Camera(WindowFrameSize frameSize, vec3f position, vec3f target) : m_Film(frameSize), m_Position(position) {
    ChangeAspectRation(frameSize);
    UpdateSupportVectors();
}

void Camera::ChangeAspectRation(WindowFrameSize& frameSize) {
    m_Film = frameSize;
    m_AspectRatio = (float) m_Film.width / (float) m_Film.height;

    UpdateSupportVectors();
}

Ray Camera::GetPrimaryRay(float x, float y) {
    const float PixelX = x * (1.0f / (float) m_Film.width);
    const float PixelY = y * (1.0f / (float) m_Film.height);

    const float ScreenX = 2.f * PixelX - 1.f;
    const float ScreenY = 1.f - 2.f * PixelY;

    vec3f pixelPosition = m_Target + m_Left * ScreenX + m_Top * ScreenY;

    return {m_Position, normalize(pixelPosition)};
}

void Camera::UpdateSupportVectors() {
    m_Distance = tanf((m_Fov * 0.5f) * (M_PI / 180.0f));
    m_Target = CalculateViewDirection();

    vec3f Ahead = m_Target;
    vec3f Right = normalize(cross(Ahead, m_Up));
    vec3f Up = normalize(cross(Right, Ahead));

    m_Left = Right * m_Distance * m_AspectRatio;
    m_Top = Up * m_Distance;
}

void Camera::MoveForward(float deltaTime) {
    m_Position -= m_Target * m_MoveSensitivity * deltaTime;
    UpdateSupportVectors();
}

void Camera::MoveBackward(float deltaTime) {
    m_Position += m_Target * m_MoveSensitivity * deltaTime;
    UpdateSupportVectors();
}

void Camera::MoveUp(float deltaTime) {
    m_Position += m_Up * m_MoveSensitivity * deltaTime;

    UpdateSupportVectors();
}

void Camera::MoveDown(float deltaTime) {
    m_Position -= m_Up * m_MoveSensitivity * deltaTime;

    UpdateSupportVectors();
}

void Camera::MoveRight(float deltaTime) {
    const vec3f Right = normalize(cross(m_Target, m_Up));
    m_Position += Right * m_MoveSensitivity * deltaTime;

    UpdateSupportVectors();
}

void Camera::MoveLeft(float deltaTime) {
    const vec3f Right = normalize(cross(m_Target, m_Up));
    m_Position -= Right * m_MoveSensitivity * deltaTime;

    UpdateSupportVectors();
}

void Camera::ChangeOrientation(float xOffset, float yOffset, float elapsedTime) {
    xOffset *= m_RotationSensitivity;
    yOffset *= m_RotationSensitivity;

    m_Pitch += yOffset * elapsedTime;
    m_Yaw += xOffset * elapsedTime;

    UpdateSupportVectors();
}

vec3f Camera::CalculateViewDirection() const {
    return {sinf(m_Yaw) * cosf(m_Pitch), sinf(m_Pitch), -1.0f * cosf(m_Yaw) * cosf(m_Pitch)};
}

void Camera::ChangeFov(float fov) {
    if(fov < 40.0f) {
        fov = 40.0f;
    }

    if(fov > 120.f) {
        fov = 120.f;
    }

    m_Fov = fov;
    UpdateSupportVectors();
}

const float Camera::GetFieldOfView() const {
    return m_Fov;
}


