//
// Created by Ian on 7-12-2023.
//

#ifndef ADVANCEDGRAPHICS_FRAMERENDERER_H
#define ADVANCEDGRAPHICS_FRAMERENDERER_H

#include <webgpu/webgpu.h>
#include "RenderContext.h"

class FrameRenderer {
    ContextResources m_ContextResources;
    WindowFrameSize m_Film;

    WGPURenderPipeline m_pipeline = nullptr;

    WGPUBuffer m_VertexBuffer = nullptr;
    WGPUBuffer m_IndexBuffer = nullptr;

    WGPUBindGroupLayout m_bindGroupLayout = nullptr;
    WGPUBindGroup m_BindGroup = nullptr;

    WGPUTexture m_AccumulatorTexture = nullptr;
    WGPUSampler m_AccumulatorSampler = nullptr;
    WGPUTextureView m_AccumulatorTextureView = nullptr;

    void BuildPipeline();
    void BuildBuffers();
    void BuildAccumulatorTexture(uint32_t, uint32_t);

    std::string ReadShaderCode(const char * filePath);
public:
    FrameRenderer() = default;
    FrameRenderer(FrameRenderer&&) = default;
    FrameRenderer(FrameRenderer&) = default;
    ~FrameRenderer();

    void Init(const ContextResources&, WindowFrameSize);
    void Draw(const WGPURenderPassEncoder*);

    void ResizeAccumulatorTexture(WindowFrameSize);
    void CopyAccumulatorToTexture(uint32_t*);
};


#endif //ADVANCEDGRAPHICS_FRAMERENDERER_H
