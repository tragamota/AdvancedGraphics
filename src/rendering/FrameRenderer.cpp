//
// Created by Ian on 7-12-2023.
//

#include "FrameRenderer.h"

#include <array>
#include <fstream>
#include <sstream>

const std::array<float, 16> FRAME_DATA = {
    -1.0f, -1.0f, 0.0f, 0.0f,
    -1.0f,  1.0f, 0.0f, 1.0f,
     1.0f,  1.0f, 1.0f, 1.0f,
     1.0f, -1.0f, 0.0f, 1.0f
};

const std::array<uint32_t, 6> FRAME_INDICES = {
    0, 1, 2, 0, 3, 2
};

FrameRenderer::FrameRenderer(ContextResources& resources) : m_ContextResources(resources) {
    BuildBuffers();
    BuildPipeline();
}

FrameRenderer::~FrameRenderer() {
//    if(m_VertexBuffer != nullptr) {
//        wgpuBufferRelease(m_VertexBuffer);
//        wgpuBufferDestroy(m_VertexBuffer);
//    }
//
//    if(m_IndexBuffer != nullptr) {
//        wgpuBufferRelease(m_IndexBuffer);
//        wgpuBufferDestroy(m_IndexBuffer);
//    }
}

inline void FrameRenderer::BuildBuffers() {
    WGPUBufferDescriptor bufferDesc = {
        .usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Vertex,
        .size = FRAME_DATA.size() * sizeof(float),
        .mappedAtCreation = false,
    };

    m_VertexBuffer = wgpuDeviceCreateBuffer(*m_ContextResources.device, &bufferDesc);
    wgpuQueueWriteBuffer(*m_ContextResources.queue, m_VertexBuffer, 0, FRAME_DATA.data(), bufferDesc.size);

    bufferDesc = {
        .usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Index,
        .size =  FRAME_INDICES.size() * sizeof(uint32_t),
        .mappedAtCreation = false
    };

    m_IndexBuffer = wgpuDeviceCreateBuffer(*m_ContextResources.device, &bufferDesc);
    wgpuQueueWriteBuffer(*m_ContextResources.queue, m_IndexBuffer, 0, FRAME_INDICES.data(), bufferDesc.size);
}

inline void FrameRenderer::BuildPipeline() {
    std::array<WGPUVertexAttribute, 2> attributes {};
    std::string shader = ReadShaderCode("shaders/basic.wgsl");

    attributes[0] = {
        .format = WGPUVertexFormat_Float32x2,
        .offset = 0,
        .shaderLocation = 0,
    };

    attributes[1] = {
        .format = WGPUVertexFormat_Float32x2,
        .offset = 2 * sizeof(float),
        .shaderLocation = 1
    };

    WGPUVertexBufferLayout vertexBufferLayout = {
        .arrayStride = 4 * sizeof(float),
        .stepMode = WGPUVertexStepMode_Vertex,
        .attributeCount = attributes.size(),
        .attributes = attributes.data(),
    };

    WGPUShaderModuleWGSLDescriptor shaderDescriptor = {
        .chain =  {
            .next = nullptr,
            .sType = WGPUSType_ShaderModuleWGSLDescriptor
        },
        .code = shader.c_str()
    };

    WGPUShaderModuleDescriptor shaderModuleDescriptor {
        .nextInChain = &shaderDescriptor.chain
    };

    auto shaderModule = wgpuDeviceCreateShaderModule(*m_ContextResources.device, &shaderModuleDescriptor);

    WGPUBlendState blendState = {
        .color = {
                .operation = WGPUBlendOperation_Add,
                .srcFactor = WGPUBlendFactor_SrcAlpha,
                .dstFactor = WGPUBlendFactor_OneMinusSrcAlpha,
        },
        .alpha = {
                .operation = WGPUBlendOperation_Add,
                .srcFactor = WGPUBlendFactor_Zero,
                .dstFactor = WGPUBlendFactor_One
        }
    };

    WGPUColorTargetState colorTarget = {
        .nextInChain = nullptr,
        .format = m_ContextResources.surfaceFormat,
        .blend = &blendState,
        .writeMask = WGPUColorWriteMask_All
    };

    WGPUFragmentState fragmentState = {
        .nextInChain = nullptr,
        .module = shaderModule,
        .entryPoint = "frag_main",
        .constantCount = 0,
        .constants = nullptr,
        .targetCount = 1,
        .targets = &colorTarget,
    };

    WGPURenderPipelineDescriptor pipelineDesc {
        .nextInChain = nullptr,
        .layout = nullptr,
        .vertex = {
            .nextInChain = nullptr,
            .module = shaderModule,
            .entryPoint = "vert_main",
            .constantCount = 0,
            .constants = nullptr,
            .bufferCount = 1,
            .buffers = &vertexBufferLayout,
        },
        .primitive = {
            .nextInChain = nullptr,
            .topology = WGPUPrimitiveTopology_TriangleList,
            .stripIndexFormat = WGPUIndexFormat_Undefined,
            .cullMode = WGPUCullMode_None,
        },
        .depthStencil = nullptr,
        .multisample = {
            .count = 1,
            .mask = ~0u,
            .alphaToCoverageEnabled = false
        },
        .fragment = &fragmentState
    };

    m_pipeline = wgpuDeviceCreateRenderPipeline(*m_ContextResources.device, &pipelineDesc);
}

void FrameRenderer::Draw(const WGPURenderPassEncoder *encoder) {
    wgpuRenderPassEncoderSetPipeline(*encoder, m_pipeline);

    wgpuRenderPassEncoderSetVertexBuffer(*encoder, 0, m_VertexBuffer, 0, 64);
    wgpuRenderPassEncoderSetIndexBuffer(*encoder, m_IndexBuffer, WGPUIndexFormat_Uint32, 0, 24);

    wgpuRenderPassEncoderDrawIndexed(*encoder, 6, 1,0, 0, 0);
}

std::string FrameRenderer::ReadShaderCode(const char *filePath) {
    std::stringstream buffer;
    std::ifstream inputFile(filePath);

    if (!inputFile.is_open())
        throw std::runtime_error("Shader file doesn't exist");

    buffer << inputFile.rdbuf();

    inputFile.close();

    return buffer.str();
}

