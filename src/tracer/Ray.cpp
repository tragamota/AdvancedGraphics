//
// Created by Ian on 2-1-2024.
//

#include "Ray.h"

#include "../utils/Math.h"

Ray::Ray(vec3f origin) : m_Origin(origin), m_Direction(0, 0, 1) {

}

Ray::Ray(vec3f origin, vec3f direction) : m_Origin(origin), m_Direction(direction){

}

Ray Ray::At(float t) {
    auto newOrigin = m_Origin + m_Direction * t;

    return {newOrigin, m_Direction};
}

Ray& Ray::Normalize() {
    m_Direction = normalize(m_Direction);

    return *this;
}

const vec3f& Ray::GetOrigin() const {
    return m_Origin;
}

const vec3f& Ray::GetDirection() const {
    return m_Direction;
}
