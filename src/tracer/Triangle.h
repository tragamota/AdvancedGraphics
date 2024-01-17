//
// Created by Ian on 8-1-2024.
//

#ifndef ADVANCEDGRAPHICS_TRIANGLE_H
#define ADVANCEDGRAPHICS_TRIANGLE_H


#include "../utils/Math.h"

class Triangle {
public:
    vec3f vertexOne, VertexTwo, VertexThree;
    vec3f centroid;

    Triangle(vec3f& vertexOne, vec3f& vertexTwo, vec3f& vertexThree);

    void CalculateCentroid();
};

class alignas(64) TriangleExt {
    vec2f UvOne, UVTwo, UVThree;
    vec3f NormalOne, NormalTwo, NormalThree;
};


#endif //ADVANCEDGRAPHICS_TRIANGLE_H
