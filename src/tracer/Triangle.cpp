//
// Created by Ian on 8-1-2024.
//

#include "Triangle.h"

constexpr const float ONE_THIRD = 1.f / 3.f;

Triangle::Triangle(vec3f& vertexOne, vec3f& vertexTwo, vec3f& vertexThree) : vertexOne(vertexOne), VertexTwo( vertexTwo), VertexThree(vertexThree) {

}

void Triangle::CalculateCentroid() {
    centroid = (vertexOne + VertexTwo + VertexThree);
    centroid *= ONE_THIRD;
}
