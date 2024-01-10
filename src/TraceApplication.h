//
// Created by Ian on 17-11-2023.
//

#ifndef ADVANCEDGRAPHICS_TRACEAPPLICATION_H
#define ADVANCEDGRAPHICS_TRACEAPPLICATION_H

#include "Interface.h"

#include "core/Application.h"
#include "rendering/RenderContext.h"
#include "rendering/FrameRenderer.h"
#include "tracer/Camera.h"
#include "tracer/Accumulator.h"
#include "tracer/PathTracer.h"

struct ControlStates {
    bool enableMouse;
    bool hasToggledEnableMouse;
};

class TraceApplication : public Application {
public:
    void Init() override;
    void OnTick(float elapsedTime) override;
    void ShutDown() override;
private:
    ControlStates controlStates {};

    RenderContext m_Context;
    FrameRenderer m_FrameRenderer;
    Interface m_Interface;

    PathTracer* m_PathTracer;
    Accumulator* m_Accumulator;
    Camera* m_Camera;

    vec2f mousePosition;
    uint8_t keyState[GLFW_KEY_LAST];

    bool m_ResizeRequested = false;

    void Update(float);
    void Render();
};


#endif //ADVANCEDGRAPHICS_TRACEAPPLICATION_H
