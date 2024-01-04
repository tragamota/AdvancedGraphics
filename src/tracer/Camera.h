//
// Created by Ian on 3-1-2024.
//

#ifndef ADVANCEDGRAPHICS_CAMERA_H
#define ADVANCEDGRAPHICS_CAMERA_H

#include "Ray.h"
#include "../utils/Math.h"

class Camera {
    vec3f m_Position;
    vec3f m_Target;

    vec3f m_TopLeft;
    vec3f m_TopRight;
    vec3f m_BottomLeft;

    float m_aspectRatio;

public:
    Camera() : m_Origin(0), m_Direction(1, 0, 0) {
        
    };

};


#endif //ADVANCEDGRAPHICS_CAMERA_H
