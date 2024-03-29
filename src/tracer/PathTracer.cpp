//
// Created by Ian on 6-1-2024.
//

#include "PathTracer.h"
#include "../utils/Constants.h"

#include <math.h>

PathTracer::PathTracer(const char *skyboxPath, const WindowFrameSize frameSize) : m_Film(frameSize) {
    skyBoxTexture = new Texture(skyboxPath);
    skyBoxTexture->ClampToneMap();

    m_Camera = Camera(frameSize, vec3f(0, 0, 1), vec3f(0, 0,  0));
    m_Accumulator = new Accumulator(frameSize);
}

PathTracer::~PathTracer() {
    delete skyBoxTexture;
    delete m_Accumulator;
}

void PathTracer::Resize(WindowFrameSize windowFrameSize) {
    m_Film = windowFrameSize;
    m_Accumulator->Resize(windowFrameSize);
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
    #pragma omp parallel for schedule(dynamic)
    for(int y = 0; y < m_Film.height; y++) {
        #pragma omp parallel for schedule(dynamic)
        for(int x = 0; x < m_Film.width; x++) {
            auto ray = m_Camera.GetPrimaryRay(x, y);
            auto& rayD = ray.GetDirection();

            auto u = static_cast<int>(skyBoxTexture->GetWidth() * atan2f( rayD.z, rayD.x ) * INV2PI - 0.5f);
            auto v = static_cast<int>(skyBoxTexture->GetHeight() * acosf( rayD.y ) * INVPI - 0.5f);

            auto sampleColor = skyBoxTexture->Sample(u, v);

            m_Accumulator->AccumulateValue(sampleColor, x, y);
        }
    }
}

vec4f PathTracer::Trace(Ray& ray, RayTraceInfo& traceInfo) {

}

