//
// Created by Ian on 13-1-2024.
//

#ifndef ADVANCEDGRAPHICS_BONSAIBVH_H
#define ADVANCEDGRAPHICS_BONSAIBVH_H


#include <vector>

#include "Triangle.h"
#include "BonsaiMiniTreeSelector.h"
#include "Ray.h"

class BonsaiBVH {
    Triangle* m_Triangles;
    uint32_t m_TriangleCount;

    BonsaiMiniTreeSelector* m_TreeSelector;

    void CalculateTriangleCentroids();
    void MiniTreeSelection();
    void BuildMiniTrees();
    void ConstructTopLevel();
public:
    explicit BonsaiBVH(std::vector<Triangle>& triangles);

    void BuildBvh();
    void Traverse(Ray& ray, RayTraceInfo& info);
};


#endif //ADVANCEDGRAPHICS_BONSAIBVH_H
