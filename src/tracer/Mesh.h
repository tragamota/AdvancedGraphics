//
// Created by Ian on 16-1-2024.
//

#ifndef ADVANCEDGRAPHICS_MESH_H
#define ADVANCEDGRAPHICS_MESH_H


#include "BVH.h"
#include "Texture.h"

class Mesh {
    std::vector<Triangle> m_Triangles;
    std::vector<TriangleExt> m_TriangleExternals;

    // BVH m_Bvh;

    Texture* m_Diffuse;
    Texture* m_Specular;
    Texture* m_Normal;

public:
    Mesh() = default;
    Mesh(std::vector<Triangle> triangles, std::vector<TriangleExt> triangleExternal, Texture* diffuse, Texture* Specular, Texture* normal);
};


#endif //ADVANCEDGRAPHICS_MESH_H
