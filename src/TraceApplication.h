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

    vec2f lastMousePosition;
    vec2f currentMousePosition;
    bool leftMouseButtonPressed;
    uint8_t keyState[GLFW_KEY_LAST];

    bool m_ResizeRequested = false;
};

class TraceApplication : public Application {
public:
    void Init() override;
    void OnTick(float elapsedTime) override;
    void ShutDown() override;
private:
    ControlStates controlStates {};

    RenderContext m_Context;
    Interface m_Interface;

    FrameRenderer* m_FrameRenderer;

    PathTracer* m_PathTracer;
    Accumulator* m_Accumulator;
    Camera* m_Camera;

    void Update(float);
    void Render();
};


#endif //ADVANCEDGRAPHICS_TRACEAPPLICATION_H
