//
// Created by Ian on 7-12-2023.
//

#include "FrameRenderer.h"

constexpr float FRAME_POINTS[8] = {
        -1.0f, -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f
};

constexpr float FRAME_UV[8] = {
        -1.0f, -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f
};

FrameRenderer::FrameRenderer(WGPUDevice* device, WGPURenderPipeline* pipeline) : m_RenderDevice(device), m_RenderPipeline(pipeline) {

}

void FrameRenderer::BuildBuffers() {

}

void FrameRenderer::RenderFrame(WGPUCommandEncoder *encoder) {

}

FrameRenderer::~FrameRenderer() {
    wgpuBufferDestroy(m_VertexBuffer);

    wgpuBufferRelease(m_VertexBuffer);
}


