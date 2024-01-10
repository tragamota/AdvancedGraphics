//
// Created by Ian on 3-1-2024.
//

#define _USE_MATH_DEFINES

#include "Camera.h"
#include <cmath>


Camera::Camera(WindowFrameSize frameSize) : m_Film(frameSize), m_Position(0, 0, 0), m_Target(0, 0, 1) {
    ChangeAspectRation(frameSize);
    UpdateSupportVectors();
}

Camera::Camera(WindowFrameSize frameSize, vec3f position, vec3f target) : m_Film(frameSize), m_Position(position), m_Target(target) {
    ChangeAspectRation(frameSize);
    UpdateSupportVectors();
}

void Camera::ChangeAspectRation(WindowFrameSize& frameSize) {
    m_Film = frameSize;
    m_AspectRatio = (float) m_Film.width / (float) m_Film.height;
}

Ray Camera::GetPrimaryRay(int width, int height) {
    float u = static_cast<float>(width) / static_cast<float>(m_Film.width - 1);
    float v = static_cast<float>(height) / static_cast<float>(m_Film.height - 1);

    vec3f pixelPosition = m_TopLeft * (1.0f - u) * (1.0f - v) + m_TopRight * u * (1.0f - v) + m_BottomLeft * (1.0f - u) * v;
    vec3f rayDirection = normalize(pixelPosition - m_Position);

    return {m_Position, rayDirection};
}

void Camera::UpdateSupportVectors() {
    vec3f view = normalize(m_Target - m_Position);
    vec3f right = normalize(cross(view, vec3f(0, 1, 0)));
    vec3f up = cross(right, view);

    float halfWidth = std::tan(0.5f * 90.0 * M_PI / 180.0f);
    float halfHeight = std::tan(0.5f * m_AspectRatio / 90.0 * M_PI / 180.0f);

    m_TopLeft = m_Position + view + up * halfHeight - right * halfWidth;
    m_TopRight = m_Position + view + up * halfHeight + right * halfWidth;
    m_BottomLeft = m_Position + view - up * halfHeight - right * halfWidth;
}

void Camera::MoveForward(float deltaTime) {
    m_Position -= normalize(m_Target - m_Position) * m_MoveSensitivity * deltaTime;
}

void Camera::MoveRight(float deltaTime) {
    m_Position -= normalize(cross((m_Target - m_Position), vec3f(0, 1, 0))) * m_MoveSensitivity * deltaTime;
}

void Camera::MoveBackward(float deltaTime) {
    m_Position += normalize(m_Target - m_Position) * m_MoveSensitivity * deltaTime;
}

void Camera::MoveLeft(float deltaTime) {
    m_Position += normalize(cross(m_Target - m_Position, (vec3f(0, 1, 0)))) * m_MoveSensitivity * deltaTime ;
}

void Camera::MoveUp(float deltaTime) {

}


void Camera::MoveDown(float deltaTime) {

}

void Camera::ChangeOrientation(float xOffset, float yOffset) {
    xOffset *= m_RotationSensitivity;
    yOffset *= m_RotationSensitivity;

    vec3f view = normalize(m_Target - m_Position);
    vec3f right = normalize(cross(view, vec3f(0, 1, 0)));
    vec3f up = cross(right, view);

    m_Target += up * yOffset;
    m_Target += right * xOffset;

    UpdateSupportVectors();
}


