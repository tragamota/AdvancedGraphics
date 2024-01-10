//
// Created by Ian on 2-1-2024.
//

#ifndef ADVANCEDGRAPHICS_RAY_H
#define ADVANCEDGRAPHICS_RAY_H


#include "../utils/Math.h"

struct RayTraceInfo {
    float t;
    vec3f InverseDirection;
};

class alignas(32) Ray {
    vec3f m_Origin;
    vec3f m_Direction;

public:
    Ray() = default;
    Ray(vec3f origin);
    Ray(vec3f origin, vec3f direction);

    Ray At(float t);
    Ray& Normalize();

    const vec3f& GetOrigin() const;
    const vec3f& GetDirection() const;
};


#endif //ADVANCEDGRAPHICS_RAY_H
