//
// Created by Ian on 6-1-2024.
//

#ifndef ADVANCEDGRAPHICS_PATHTRACER_H
#define ADVANCEDGRAPHICS_PATHTRACER_H

#include "Camera.h"
#include "Accumulator.h"
#include "Scene.h"
#include "Texture.h"

class PathTracer {
    Camera m_Camera;
    Accumulator* m_Accumulator;
    Scene m_Scene;

    Texture* skyBoxTexture;

    WindowFrameSize m_Film;

    std::vector<Ray> m_PrimaryRays;

    void GeneratePrimaryRays();
public:
    explicit PathTracer(const char* skyboxPath, const WindowFrameSize frameSize);

    uint32_t frameIndex = 0;

    void RenderFrame();

    [[nodiscard]] Accumulator* GetAccumulator();
    [[nodiscard]] Camera* GetCamera();
    [[nodiscard]] Scene* GetScene();

};


#endif //ADVANCEDGRAPHICS_PATHTRACER_H
