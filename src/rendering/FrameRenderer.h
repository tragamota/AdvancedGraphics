//
// Created by Ian on 7-12-2023.
//

#ifndef ADVANCEDGRAPHICS_FRAMERENDERER_H
#define ADVANCEDGRAPHICS_FRAMERENDERER_H

#include <webgpu/webgpu.h>
#include "RenderContext.h"

class FrameRenderer {
    ContextResources m_ContextResources { };

    WGPURenderPipeline m_pipeline = nullptr;

    WGPUBuffer m_VertexBuffer = nullptr;
    WGPUBuffer m_IndexBuffer = nullptr;

    WGPUBindGroup m_BindGroup = nullptr;

    WGPUTexture m_AccumulatorTexture = nullptr;
    WGPUTextureView m_AccumulatorTextureView = nullptr;
    WGPUSampler m_AccumulatorSampler = nullptr;

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

    void ResizeAccumulatorTexture(const int, const int);
    void CopyAccumulatorToTexture(uint8_t*);
};


#endif //ADVANCEDGRAPHICS_FRAMERENDERER_H
