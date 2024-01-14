//
// Created by Ian on 6-1-2024.
//

#include "PathTracer.h"
#include "../utils/Constants.h"

#include <cmath>

PathTracer::PathTracer(const char *skyboxPath, const WindowFrameSize frameSize) : m_Film(frameSize) {
    skyBoxTexture = new Texture(skyboxPath);

    m_Camera = Camera(frameSize, vec3f(2, 2, -2), vec3f(0, 0,  0));
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
    static int move = 0;
    m_Accumulator->Clear();
//    #pragma omp parallel for schedule(dynamic)
    for(int y = 0; y < m_Film.height; y++) {
//        #pragma omp parallel for schedule(dynamic)
        for(int x = 0; x < m_Film.width; x++) {
            //m_PrimaryRays.push_back(m_Camera.GetPrimaryRay(x, y));
            auto ray = m_Camera.GetPrimaryRay(x, y);
            auto& rayD = ray.GetDirection();

            auto u = (uint32_t) ((float) skyBoxTexture->GetWidth() * atan2f( rayD.z, rayD.x ) * INV2PI - 0.5f);
            auto v = (uint32_t) ((float) skyBoxTexture->GetHeight() * acosf( rayD.y ) * INVPI - 0.5f);

            auto sampleColor = skyBoxTexture->Sample(1366, 625);

            m_Accumulator->AccumulateValue(sampleColor, x, y);
        }
    }

    move++;
}

PathTracer::~PathTracer() {
    delete skyBoxTexture;
    delete m_Accumulator;
}

void PathTracer::Resize(WindowFrameSize windowFrameSize) {
    m_Film = windowFrameSize;
    m_Accumulator->Resize(windowFrameSize);
}
