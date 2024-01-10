//
// Created by Ian on 4-1-2024.
//

#include "AABB.h"

#include <xmmintrin.h>

AABB::AABB(vec3f min, vec3f max) : m_Min(min), m_Max(max) {
    mMin = _mm_set_ps(min.x, min.y, min.z, 0);
    mMax = _mm_set_ps(max.x, max.y, max.z, 0 );
}

vec3f AABB::Center() const {
    return (m_Min + m_Max) * 0.5f;
}

__m128 AABB::CenterSimd() const {
    __m128 result;
    return result;
}

bool AABB::IsDegenerate() const {
    return false;
}

bool AABB::IsEmpty() const {
    return false;
}

float AABB::Min(const unsigned int axis) {
    return std::min(m_Min.xyz[axis], m_Max.xyz[axis]);
}

float AABB::Max(const unsigned int axis) {
    return std::min(m_Max.xyz[axis], m_Max.xyz[axis]);
}

float AABB::Area() const {
    return 0;
}

float AABB::Volume() const {
    return 0;
}


