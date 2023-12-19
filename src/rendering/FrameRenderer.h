//
// Created by Ian on 7-12-2023.
//

#ifndef ADVANCEDGRAPHICS_FRAMERENDERER_H
#define ADVANCEDGRAPHICS_FRAMERENDERER_H

#include <webgpu/webgpu.h>
#include "RenderContext.h"

class FrameRenderer {
    ContextResources m_ContextResources;

    WGPUBuffer m_VertexBuffer = nullptr;
    WGPUBuffer m_IndexBuffer = nullptr;

    WGPURenderPipeline m_pipeline;

    void BuildPipeline();
    void BuildBuffers();

    std::string ReadShaderCode(const char * filePath);
public:
    explicit FrameRenderer(ContextResources&);
    FrameRenderer() = default;
    FrameRenderer(FrameRenderer&) = default;
    ~FrameRenderer();

    void Draw(const WGPURenderPassEncoder*);
};


#endif //ADVANCEDGRAPHICS_FRAMERENDERER_H
