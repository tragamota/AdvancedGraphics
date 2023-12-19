//
// Created by Ian on 17-11-2023.
//

#ifndef ADVANCEDGRAPHICS_TRACEAPPLICATION_H
#define ADVANCEDGRAPHICS_TRACEAPPLICATION_H

#include "Interface.h"

#include "core/Application.h"
#include "rendering/RenderContext.h"
#include "rendering/FrameRenderer.h"


class TraceApplication : public Application {
public:
    void Init() override;
    void OnTick(double elapsedTime) override;
    void ShutDown() override;
private:
    RenderContext m_Context;
    Interface m_Interface;
    FrameRenderer m_FrameRenderer;

    bool m_ResizeRequested = false;

    void Update();
    void Render();
};


#endif //ADVANCEDGRAPHICS_TRACEAPPLICATION_H
