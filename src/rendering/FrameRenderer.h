//
// Created by Ian on 7-12-2023.
//

#ifndef ADVANCEDGRAPHICS_FRAMERENDERER_H
#define ADVANCEDGRAPHICS_FRAMERENDERER_H


#include <webgpu/webgpu.h>

class FrameRenderer {
    WGPUDevice* m_RenderDevice;
    WGPURenderPipeline* m_RenderPipeline;

    WGPUBuffer m_VertexBuffer;
    WGPUBuffer m_UvBuffer;

public:
    explicit FrameRenderer(WGPUDevice*, WGPURenderPipeline*);
    FrameRenderer(FrameRenderer&&) = delete;
    FrameRenderer(FrameRenderer&) = delete;
    ~FrameRenderer();

    void BuildBuffers();
    void RenderFrame(WGPUCommandEncoder* encoder);
};


#endif //ADVANCEDGRAPHICS_FRAMERENDERER_H
