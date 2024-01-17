//
// Created by Ian on 17-1-2024.
//

#ifndef ADVANCEDGRAPHICS_BONSAIMINITREESELECTOR_H
#define ADVANCEDGRAPHICS_BONSAIMINITREESELECTOR_H

#include <vector>
#include "Triangle.h"

struct AABB {
    vec3f minBounds = std::numeric_limits<float>::min();
    vec3f maxBounds = std::numeric_limits<float>::max();

    void Grow(vec3f point) {
        minBounds.x = fminf( minBounds.x, point.x );
        minBounds.y = fminf( minBounds.y, point.y );
        minBounds.z = fminf( minBounds.z, point.z );

        maxBounds.x = fminf( maxBounds.x, point.x );
        maxBounds.y = fminf( maxBounds.y, point.y );
        maxBounds.z = fminf( maxBounds.z, point.z );
    }

    void UpdateBounds(Triangle& triangle) {
        minBounds.x = fmaxf(minBounds.x,triangle.vertexOne.x);
        minBounds.y = fmaxf(minBounds.y, triangle.vertexOne.y);
        minBounds.z = fmaxf(minBounds.z,triangle.vertexOne.z);

        minBounds.x = fmaxf(minBounds.x,triangle.VertexTwo.x);
        minBounds.y = fmaxf(minBounds.y, triangle.VertexTwo.y);
        minBounds.z = fmaxf(minBounds.z,triangle.VertexTwo.z);

        minBounds.x = fmaxf(minBounds.x,triangle.VertexThree.x);
        minBounds.y = fmaxf(minBounds.y, triangle.VertexThree.y);
        minBounds.z = fmaxf(minBounds.z,triangle.VertexThree.z);

        maxBounds.x = fmaxf(maxBounds.x,triangle.vertexOne.x);
        maxBounds.y = fmaxf(maxBounds.y, triangle.vertexOne.y);
        maxBounds.z = fmaxf(maxBounds.z,triangle.vertexOne.z);

        maxBounds.x = fmaxf(maxBounds.x,triangle.VertexTwo.x);
        maxBounds.y = fmaxf(maxBounds.y, triangle.VertexTwo.y);
        maxBounds.z = fmaxf(maxBounds.z,triangle.VertexTwo.z);

        maxBounds.x = fmaxf(maxBounds.x,triangle.VertexThree.x);
        maxBounds.y = fmaxf(maxBounds.y, triangle.VertexThree.y);
        maxBounds.z = fmaxf(maxBounds.z,triangle.VertexThree.z);
    }

    float Area() {
        vec3f extent = maxBounds - minBounds; // box extent
        return extent.x * extent.y + extent.y * extent.z + extent.z * extent.x;
    }
};

struct MiniTreeSelectorNode {
    AABB bounds;
    uint32_t left, count;
};

class BonsaiMiniTreeSelector {
    Triangle* m_Triangles;
    uint32_t* m_TriangleIndices;
    uint32_t  m_TriangleCount;

    std::vector<std::vector<Triangle*>> m_MiniTrees;

    void Subdivide(MiniTreeSelectorNode& node);
    void Partition(MiniTreeSelectorNode& node, MiniTreeSelectorNode& left, MiniTreeSelectorNode& right);
    void UpdateBounds(MiniTreeSelectorNode&);
public:
    explicit BonsaiMiniTreeSelector(Triangle*, uint32_t triangleCount);
    ~BonsaiMiniTreeSelector();

    void SelectMiniTrees();
    std::vector<std::vector<Triangle*>> GetMiniTrees();
};


#endif //ADVANCEDGRAPHICS_BONSAIMINITREESELECTOR_H
