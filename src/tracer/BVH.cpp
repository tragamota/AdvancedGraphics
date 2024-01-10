//
// Created by Ian on 2-1-2024.
//

#include "BVH.h"



BVH::BVH(std::vector<Triangle *> triangles) : m_Triangles(triangles.data()) {
    m_TriangleCount = triangles.size();

    m_TriangleIndices =  new uint32_t[triangles.size()];
    m_BVHNodes = new BVHNode[triangles.size() * 2 - 1];
}

void BVH::Build() {
    for(int i = 0; i < m_TriangleCount; i++) {
        m_TriangleIndices[i] = i;
        m_Triangles[i]->CalculateCentroid();
    }

    m_BVHNodes->left = 0,
    m_BVHNodes->count = m_TriangleCount;

    UpdateBounds(m_BVHNodes);
    BuildVolumes(0);
}

void BVH::BuildVolumes(uint32_t nodeIdx) {
    BVHNode* node = &m_BVHNodes[nodeIdx];

    int axis = 0;
    int triangleIdx, halfWaySplitIdx, leftCount = 0;
    float splitPosition = 0;

    auto splitPlaneInfo = BestSplitPlane(node);

    axis = splitPlaneInfo.axis;
    splitPosition = splitPlaneInfo.position;

    triangleIdx = node->left;
    halfWaySplitIdx = triangleIdx + node->count - 1;

    float currentCost = node->bounds.Area() * node->count;

    if (splitPlaneInfo.cost >= currentCost)
        return;

    while(triangleIdx <= halfWaySplitIdx) {
        if(m_Triangles[triangleIdx]->centroid[axis] < splitPosition) {
            triangleIdx++;
            continue;
        }

        std::swap(m_TriangleIndices[triangleIdx], m_TriangleIndices[halfWaySplitIdx--]);
    }

    leftCount = triangleIdx - node->left;

    if(leftCount == 0 || leftCount == node->count)
        return;

    int leftChildIdx = m_UsedNodes++;
    int rightChildIdx = m_UsedNodes++;

    auto leftChild = &m_BVHNodes[leftChildIdx];
    auto rightChild = &m_BVHNodes[rightChildIdx];

    leftChild->left = node->left;
    leftChild->count = leftCount;
    rightChild->left = triangleIdx;
    rightChild->count = node->count - leftCount;

    UpdateBounds( leftChild );
    UpdateBounds( rightChild );

    BuildVolumes( leftChildIdx );
    BuildVolumes( rightChildIdx );
}

void BVH::UpdateBounds(BVHNode* node) {

    for (uint32_t first = node->left, i = 0; i < node->count; i++) {
        node->bounds.UpdateBounds(m_Triangles[first]);
    }
}

SplitPlaneInfo BVH::BestSplitPlane(BVHNode *node) {
    uint32_t bestAxis = 0;
    float bestPos = 0;
    float bestCost = std::numeric_limits<float>::max();

    vec3f extent = node->bounds.minBounds - node->bounds.maxBounds;

    if(extent.y > extent.x) {
        bestAxis = 1;
    }
    if(extent.z > extent[bestAxis]) {
        bestAxis = 2;
    }

    float minBounds = node->bounds.minBounds[bestAxis];
    float maxBounds = node->bounds.maxBounds[bestAxis];

    if (minBounds == maxBounds)
        return {
            .axis = bestAxis,
            .cost = bestCost,
            .position = 0
        };

    float scale = (maxBounds - minBounds) / 16;

    for (uint32_t i = 1; i < 16; i++) {
        float candidatePos = minBounds + i * scale;
        float cost = EvaluateSAH(node, bestAxis, candidatePos);

        if (cost < bestCost) {
            bestPos = candidatePos,
            bestCost = cost;
        }
    }

    return {
        .axis = static_cast<uint32_t>(bestAxis),
        .cost = bestCost,
        .position = bestPos
    };
}

float BVH::EvaluateSAH(BVHNode* node, int axis, float pos) {
    BVHAABB leftBound, rightBound;
    int leftCount = 0, rightCount = 0;

    for( uint32_t i = 0; i < node->count; i++)
    {
        Triangle* triangle = m_Triangles[m_TriangleIndices[node->left + i]];
        if (triangle->centroid[axis] < pos)
        {
            leftCount++;
            leftBound.Grow( triangle->vertexOne);
            leftBound.Grow( triangle->VertexTwo);
            leftBound.Grow( triangle->VertexThree);
        }
        else
        {
            rightCount++;
            rightBound.Grow( triangle->vertexOne);
            rightBound.Grow( triangle->VertexTwo);
            rightBound.Grow( triangle->VertexThree);
        }
    }
    float cost = (float) leftCount * leftBound.Area() + (float) rightCount * rightBound.Area();

    return cost > 0 ? cost : std::numeric_limits<float>::max();
}


void BVH::Traverse(Ray& ray, RayTraceInfo& traceInfo) {
    BVHNode* node = &m_BVHNodes[0], *stack[64];
    uint32_t stackPtr = 0;
    while (true)
    {
        if (node->isLeaf())
        {
            for (uint32_t i = 0; i < node->count; i++)
                //IntersectTri( ray, tri[triIdx[node->leftFirst + i]] );

            if (stackPtr == 0)
                break;
            else
                node = stack[--stackPtr];

            continue;
        }

        BVHNode* leftChild = &m_BVHNodes[node->left];
        BVHNode* rightChild = &m_BVHNodes[node->left + 1];
        float dist1 = IntersectNode(ray, traceInfo, leftChild->bounds);
        float dist2 = IntersectNode(ray, traceInfo, rightChild->bounds);

        if (dist1 > dist2) {
            std::swap( dist1, dist2 );
            std::swap( leftChild, rightChild );
        }

        if (dist1 == std::numeric_limits<float>::max())
        {
            if (stackPtr == 0)
                break;
            else
                node = stack[--stackPtr];
        }
        else
        {
            node = leftChild;

            if (dist2 != std::numeric_limits<float>::max())
                stack[stackPtr++] = rightChild;
        }
    }
}

float BVH::IntersectNode(Ray &ray, RayTraceInfo& traceInfo, BVHAABB &bounds) const {
    auto& rayOrigin = ray.GetOrigin();
    auto& rayDirection = ray.GetDirection();
    auto& rayInverse = traceInfo.InverseDirection;

    float tx1 = (bounds.minBounds.x - rayOrigin.x) / rayInverse.x;
    float tx2 = (bounds.maxBounds.x - rayOrigin.x) / rayInverse.x;

    float tmin = std::min( tx1, tx2 );
    float tmax = std::max( tx1, tx2 );

    float ty1 = (bounds.maxBounds.y - rayOrigin.y) / rayInverse.y;
    float ty2 = (bounds.minBounds.y - rayOrigin.y) / rayInverse.y;

    tmin = std::max( tmin, std::min( ty1, ty2 ) ),
    tmax = std::min( tmax, std::max( ty1, ty2 ) );

    float tz1 = (bounds.minBounds.z - rayOrigin.z) / rayInverse.z;
    float tz2 = (bounds.maxBounds.z - rayOrigin.z) / rayInverse.z;

    tmin = std::max( tmin, std::min( tz1, tz2 ) );
    tmax = std::min( tmax, std::max( tz1, tz2 ) );

    if (tmax >= tmin && tmin < traceInfo.t && tmax > 0)
        return tmin;
    else
        return std::numeric_limits<float>::max();
}


