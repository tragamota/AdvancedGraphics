//
// Created by Ian on 13-1-2024.
//

#ifndef ADVANCEDGRAPHICS_BONSAIBVH_H
#define ADVANCEDGRAPHICS_BONSAIBVH_H


#include <vector>
#include "Triangle.h"

struct alignas(64) MiniTreeSelection {
    vec3f minBounds, maxBounds;
    uint32_t left, count;
};

class BonsaiBVH {
    std::vector<Triangle> m_Triangles;

    uint32_t m_TriangleCount;
    uint32_t* m_MiniTreeSelectionIndices;
public:
    BonsaiBVH(std::vector<Triangle> triangles);

    void BuildBvh();
    void CalculateTriangleCentroids();

    void UpdateMiniSelectionBounds(MiniTreeSelection& treeSelection);
    std::vector<MiniTreeSelection> GroupMiniTrees();

};


#endif //ADVANCEDGRAPHICS_BONSAIBVH_H
