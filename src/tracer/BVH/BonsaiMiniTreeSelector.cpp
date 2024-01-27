//
// Created by Ian on 17-1-2024.
//

#include "BonsaiMiniTreeSelector.h"

BonsaiMiniTreeSelector::BonsaiMiniTreeSelector(Triangle* triangles, uint32_t triangleCount) : m_Triangles(triangles), m_TriangleCount(triangleCount) {
    m_TriangleIndices = new uint32_t[triangleCount];
}

BonsaiMiniTreeSelector::~BonsaiMiniTreeSelector() {
    delete m_TriangleIndices;
}

void BonsaiMiniTreeSelector::SelectMiniTrees() {
    for(uint32_t i = 0; i < m_TriangleCount; i++) {
        m_TriangleIndices[i] = i;
    }

    MiniTreeSelectorNode MiniTreeSelectionNode {
      .left = 0, .count = m_TriangleCount
    };

    UpdateBounds(MiniTreeSelectionNode);
    Subdivide(MiniTreeSelectionNode);
}

void BonsaiMiniTreeSelector::Subdivide(MiniTreeSelectorNode &node) {
    if(node.count < 512) {
        if(node.count <= 0)
            return;

        std::vector<Triangle*> miniTreeTriangles;

        for(uint32_t i = node.left; i < node.left + node.count; i++) {
            miniTreeTriangles.emplace_back(&m_Triangles[m_TriangleIndices[i]]);
        }

        m_MiniTrees.push_back(miniTreeTriangles);

        return;
    }

    MiniTreeSelectorNode left;
    MiniTreeSelectorNode right;

    Partition(node, left, right);

    UpdateBounds(left);
    UpdateBounds(right);

    Subdivide(left);
    Subdivide(right);
}

inline void BonsaiMiniTreeSelector::UpdateBounds(MiniTreeSelectorNode& node) {
    for(uint32_t i = node.left; i < node.count; i++)
        node.bounds.UpdateBounds(m_Triangles[i]);
}

std::vector<std::vector<Triangle*>> BonsaiMiniTreeSelector::GetMiniTrees() {
    return m_MiniTrees;
}

void BonsaiMiniTreeSelector::Partition(MiniTreeSelectorNode &node, MiniTreeSelectorNode& left, MiniTreeSelectorNode& right) {
    int bestAxis = 0;

    vec3f extent = node.bounds.maxBounds - node.bounds.minBounds;

    if(extent.y > extent.x)
        bestAxis = 1;

    if(extent.z > extent[bestAxis])
        bestAxis = 2;

    float splitPos = node.bounds.minBounds[bestAxis] + extent[bestAxis] * 0.5f;

    int i = node.left;
    int j = i + node.count - 1;
    while(i <= j) {
        if(m_Triangles[m_TriangleIndices[i]].centroid[bestAxis] < splitPos)
            i++;
        else
            std::swap(m_TriangleIndices[i], m_TriangleIndices[j--]);
    }

    left.left = node.left;
    left.count = i - node.left;

    right.left = i;
    right.count = node.count - left.count;
}


