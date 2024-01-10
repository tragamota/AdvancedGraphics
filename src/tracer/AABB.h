//
// Created by Ian on 4-1-2024.
//

#ifndef ADVANCEDGRAPHICS_AABB_H
#define ADVANCEDGRAPHICS_AABB_H


#include <complex>
#include "../utils/Math.h"

class AABB {
    union  {
        struct {
            union { struct { vec3f m_Min; float d0; }; __m128 mMin; };
            union { struct { vec3f m_Max; float d1; }; __m128 mMax; };
        };
    };

public:
    AABB() = default;
    AABB(vec3f min, vec3f max);

    [[nodiscard]] vec3f Center() const;
    [[nodiscard]] __m128 CenterSimd() const;
    [[nodiscard]] bool IsEmpty() const;
    [[nodiscard]] bool IsDegenerate() const;
    [[nodiscard]] float Volume() const;
    [[nodiscard]] float Area() const;

    float Min(unsigned int axis);
    float Max(unsigned int axis);
};


#endif //ADVANCEDGRAPHICS_AABB_H
