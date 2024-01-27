//
// Created by Ian on 13-1-2024.
//

#include "BonsaiBVH.h"

BonsaiBVH::BonsaiBVH(std::vector<Triangle>& triangles) {
    m_Triangles = triangles.data();
    m_TriangleCount = triangles.size();
}

void BonsaiBVH::BuildBvh() {
    CalculateTriangleCentroids();

    MiniTreeSelection();
    BuildMiniTrees();
}

void BonsaiBVH::CalculateTriangleCentroids() {
    #pragma omp parallel for schedule(static)
    for(int i = 0; i < m_TriangleCount; i++) {
        auto& triangle = m_Triangles[i];

        triangle.CalculateCentroid();
    }
}

void BonsaiBVH::Traverse(Ray &ray, RayTraceInfo &info) {

}

void BonsaiBVH::MiniTreeSelection() {
    m_TreeSelector->SelectMiniTrees();
}

void BonsaiBVH::BuildMiniTrees() {
    #pragma omp parallel for schedule(static)
    for(auto& tree : m_TreeSelector->GetMiniTrees()) {

    }
}

void BonsaiBVH::ConstructTopLevel() {

}
