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

    m_Target = normalize(target - m_Position);

    UpdateSupportVectors();
}

void Camera::ChangeAspectRation(WindowFrameSize& frameSize) {
    m_Film = frameSize;
    m_AspectRatio = (float) m_Film.width / (float) m_Film.height;

    UpdateSupportVectors();
}

Ray Camera::GetPrimaryRay(int x, int y) {
    float u = (static_cast<float>(x)) / static_cast<float>(m_Film.width);
    float v = (static_cast<float>(y)) / static_cast<float>(m_Film.height);

    vec3f pixelPosition = m_TopLeft + (m_TopRight - m_TopLeft) * u + (m_BottomLeft - m_TopLeft) * v;
    vec3f rayDirection = normalize(pixelPosition - m_Position);

    return {m_Position, rayDirection};
}

void Camera::UpdateSupportVectors() {
    vec3f Ahead = m_Target;
    vec3f UpGuide = vec3f(0.0f, 1.0f, 0.0f);
    vec3f Right = normalize(cross(UpGuide, Ahead));
    UpGuide = normalize(cross(Ahead, Right));

    float viewPlaneDistance = 1.0f;
    float fovRads = m_Fov * (M_PI / 180.f);
    vec3f viewPlaneCenter = m_Position + m_Target * viewPlaneDistance;

    float viewPlaneHeight = 2.0f * tan(fovRads * 0.5f); // Vertical field of view
    float viewPlaneWidth = viewPlaneHeight * m_AspectRatio; // Aspect ratio = width / height

    float halfWidth = viewPlaneWidth * 0.5f;
    float halfHeight = viewPlaneHeight * 0.5f;

    m_TopLeft = viewPlaneCenter + UpGuide * halfHeight - Right * halfWidth;
    m_TopRight = viewPlaneCenter + UpGuide * halfHeight + Right * halfWidth;
    m_BottomLeft = viewPlaneCenter - UpGuide * halfHeight - Right * halfWidth;
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
    m_Position += Up * m_MoveSensitivity * deltaTime;

    UpdateSupportVectors();
}

void Camera::MoveDown(float deltaTime) {
    m_Position -= Up * m_MoveSensitivity * deltaTime;

    UpdateSupportVectors();
}

void Camera::MoveRight(float deltaTime) {
    const vec3f Right = cross(m_Target, Up);
    m_Position -= normalize(Right) * m_MoveSensitivity * deltaTime;

    UpdateSupportVectors();
}

void Camera::MoveLeft(float deltaTime) {
    const vec3f Right = cross(m_Target, Up);
    m_Position += normalize(Right) * m_MoveSensitivity * deltaTime;

    UpdateSupportVectors();
}

void Camera::ChangeOrientation(float xOffset, float yOffset) {
    xOffset *= m_RotationSensitivity;
    yOffset *= m_RotationSensitivity;

    vec3f view = m_Target;
    vec3f right = normalize(cross(view, vec3f(0, 1, 0)));
    vec3f up = cross(right, view);

    m_Target += up * yOffset;
    m_Target += right * xOffset;

    UpdateSupportVectors();
}


