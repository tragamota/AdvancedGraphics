//
// Created by Ian on 6-1-2024.
//

#include "PathTracer.h"
#include "../utils/Constants.h"

PathTracer::PathTracer(const char *skyboxPath, const WindowFrameSize frameSize) : m_Film(frameSize) {
    skyBoxTexture = new Texture("assets/skydome_night.hdr");

    m_Camera = Camera(frameSize, vec3f(0, 0, -5), vec3f(0, -2, -2));
    m_Accumulator = new Accumulator(frameSize);
}

Scene* PathTracer::GetScene() {
    return &m_Scene;
}

Camera* PathTracer::GetCamera() {
    return &m_Camera;
}

Accumulator *PathTracer::GetAccumulator() {
    return m_Accumulator;
}

void PathTracer::RenderFrame() {
    frameIndex = 0;

    GeneratePrimaryRays();
}

void PathTracer::GeneratePrimaryRays() {
    #pragma omp parallel for schedule(dynamic)
    for(int y = 0; y < m_Film.height; y++) {
        #pragma omp parallel for schedule(dynamic)
        for(int x = 0; x < m_Film.width; x++) {
            //m_PrimaryRays.push_back(m_Camera.GetPrimaryRay(x, y));
            auto ray = m_Camera.GetPrimaryRay(x, y);

            auto& rayD = ray.GetDirection();

            uint32_t u = skyBoxTexture->GetWidth() * atan2f( rayD.z, rayD.x ) * INV2PI - 0.5f;
            uint32_t v = skyBoxTexture->GetHeight() * acosf( rayD.y ) * INVPI - 0.5f;

            auto sampleColor = skyBoxTexture->Sample(u, v);

            m_Accumulator->AccumulateValue(sampleColor, x, y);
        }
    }
}
