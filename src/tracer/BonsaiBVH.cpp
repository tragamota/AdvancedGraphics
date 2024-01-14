//
// Created by Ian on 13-1-2024.
//

#include "BonsaiBVH.h"

BonsaiBVH::BonsaiBVH(std::vector<Triangle> triangles) {
    m_Triangles = triangles;
    m_TriangleCount = triangles.size();
}

void BonsaiBVH::BuildBvh() {
    CalculateTriangleCentroids();

}

void BonsaiBVH::CalculateTriangleCentroids() {
    m_MiniTreeSelectionIndices = new uint32_t[m_TriangleCount];

    #pragma omp parallel for schedule(static)
    for(int i = 0; i < m_TriangleCount; i++) {
        auto& triangle = m_Triangles[i];

        m_MiniTreeSelectionIndices[i] = i;
        triangle.CalculateCentroid();
    }
}

std::vector<MiniTreeSelection> BonsaiBVH::GroupMiniTrees() {
    return std::vector<MiniTreeSelection>();
}

void BonsaiBVH::UpdateMiniSelectionBounds(MiniTreeSelection& treeSelection) {
//    for (uint32_t first = treeSelection.left, i = 0; i < treeSelection.count; i++) {
//        treeSelection.bounds.UpdateBounds(m_Triangles[first]);
//    }
}
//
//void BVH::UpdateBounds(BVHNode* node) {
//
//    for (uint32_t first = node->left, i = 0; i < node->count; i++) {
//        node->bounds.UpdateBounds(m_Triangles[first]);
//    }
//}