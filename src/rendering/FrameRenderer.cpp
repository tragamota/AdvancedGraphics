//
// Created by Ian on 7-12-2023.
//

#include "FrameRenderer.h"

#include <array>
#include <fstream>
#include <sstream>

constexpr const std::array<float, 16> FRAME_DATA = {
    -1.0f, -1.0f, 0.0f, 1.0f,
    -1.0f,  1.0f, 0.0f, 0.0f,
     1.0f,  1.0f, 1.0f, 0.0f,
     1.0f, -1.0f, 1.0f, 1.0f
};

constexpr const std::array<uint32_t, 6> FRAME_INDICES = {
    0, 1, 2, 0, 3, 2
};


void FrameRenderer::Init(const ContextResources& contextResources, const WindowFrameSize frameSize) {
    m_ContextResources = contextResources;

    BuildAccumulatorTexture(frameSize.width, frameSize.height);
    BuildBuffers();
    BuildPipeline();
}

FrameRenderer::~FrameRenderer() {
    wgpuBufferDestroy(m_VertexBuffer);
    wgpuBufferRelease(m_VertexBuffer);

    wgpuBufferDestroy(m_IndexBuffer);
    wgpuBufferRelease(m_IndexBuffer);

    wgpuTextureDestroy(m_AccumulatorTexture);
    wgpuTextureRelease(m_AccumulatorTexture);

    wgpuBindGroupRelease(m_BindGroup);
    wgpuTextureViewRelease(m_AccumulatorTextureView);
    wgpuSamplerRelease(m_AccumulatorSampler);
}

void FrameRenderer::BuildAccumulatorTexture(uint32_t screenWidth, uint32_t screenHeight) {
    WGPUTextureDescriptor textureDesc {
        .nextInChain = nullptr,
        .usage = WGPUTextureUsage_TextureBinding | WGPUTextureUsage_CopyDst,
        .dimension = WGPUTextureDimension_2D,
        .size = {screenWidth, screenHeight, 1 },
        .format = WGPUTextureFormat_RGBA8Unorm,
        .mipLevelCount = 1,
        .sampleCount = 1,
        .viewFormatCount = 0,
        .viewFormats = nullptr
    };

    m_AccumulatorTexture =  wgpuDeviceCreateTexture(*m_ContextResources.device, &textureDesc);

    uint8_t* pixels = new uint8_t[4 * screenWidth * screenHeight];

    for(int y = 0; y < screenHeight; y++) {
        for(int x = 0; x < screenWidth; x++) {
            uint8_t* texel = &pixels[4 * (y * screenWidth + x)];

            texel[0] = (uint8_t)  (((float) x / (float) screenWidth)  * 255.f); // r
            texel[1] = (uint8_t)  (((float) y / (float) screenHeight) * 255.f); // g
            texel[2] = 255; // b
            texel[3] = 255; // a
        }
    }

    WGPUImageCopyTexture destination {
        .nextInChain = nullptr,
        .texture = m_AccumulatorTexture,
        .mipLevel = 0,
        .origin = {0,0,0},
        .aspect = WGPUTextureAspect_All
    };

    WGPUTextureDataLayout source {
        .nextInChain = nullptr,
        .offset = 0,
        .bytesPerRow = 4 * screenWidth,
        .rowsPerImage = screenHeight,
    };

    wgpuQueueWriteTexture(*m_ContextResources.queue, &destination, pixels, 4 * screenWidth * screenHeight, &source,&textureDesc.size);

    delete[] pixels;

    WGPUTextureViewDescriptor textureViewDescriptor {
        .format = WGPUTextureFormat_RGBA8Unorm,
        .dimension = WGPUTextureViewDimension_2D,
        .baseMipLevel = 0,
        .mipLevelCount = 1,
        .baseArrayLayer = 0,
        .arrayLayerCount = 1,
        .aspect = WGPUTextureAspect_All,
    };

    m_AccumulatorTextureView = wgpuTextureCreateView(m_AccumulatorTexture, &textureViewDescriptor);

    WGPUSamplerDescriptor samplerDesc {
        .addressModeU = WGPUAddressMode_ClampToEdge,
        .addressModeV = WGPUAddressMode_ClampToEdge,
        .addressModeW = WGPUAddressMode_ClampToEdge,
        .magFilter = WGPUFilterMode_Linear,
        .minFilter = WGPUFilterMode_Linear,
        .mipmapFilter = WGPUMipmapFilterMode_Linear,
        .lodMinClamp = 0.0f,
        .lodMaxClamp = 1.0f,
        .compare = WGPUCompareFunction_Undefined,
        .maxAnisotropy = 1
    };

   m_AccumulatorSampler = wgpuDeviceCreateSampler(*m_ContextResources.device, &samplerDesc);
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

void FrameRenderer::BuildPipeline() {
    std::string shader = ReadShaderCode("shaders/basic.wgsl");

    std::array<WGPUVertexAttribute, 2> attributes {};
    std::array<WGPUBindGroupLayoutEntry, 2> bindingLayouts {};
    std::array<WGPUBindGroupEntry, 2> bindGroups {};

    WGPUVertexBufferLayout vertexBufferLayout;
    WGPUShaderModuleWGSLDescriptor shaderDescriptor;
    WGPUShaderModuleDescriptor shaderModuleDescriptor;

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

    bindingLayouts[0] = {
        .nextInChain = nullptr,
        .binding = 0,
        .visibility = WGPUShaderStage_Fragment | WGPUShaderStage_Vertex,
        .texture = {
            .nextInChain = nullptr,
            .sampleType = WGPUTextureSampleType_Float,
            .viewDimension = WGPUTextureViewDimension_2D,
        }
    };

    bindingLayouts[1] = {
        .nextInChain = nullptr,
        .binding = 1,
        .visibility = WGPUShaderStage_Fragment | WGPUShaderStage_Vertex,
        .sampler = {
            .type = WGPUSamplerBindingType_Filtering
        }
    };

    WGPUBindGroupLayoutDescriptor bindLayoutDescriptor {
        .entryCount = bindingLayouts.size(),
        .entries = bindingLayouts.data(),
    };

    WGPUBindGroupLayout bindGroupLayout = wgpuDeviceCreateBindGroupLayout(*m_ContextResources.device, &bindLayoutDescriptor);

    bindGroups[0] = {
        .binding = 0,
        .textureView = m_AccumulatorTextureView,
    };

    bindGroups[1] = {
        .binding = 1,
        .sampler = m_AccumulatorSampler
    };

    WGPUPipelineLayoutDescriptor layoutDesc {
        .bindGroupLayoutCount = 1,
        .bindGroupLayouts = &bindGroupLayout,
    };

    WGPUPipelineLayout pipelineLayout = wgpuDeviceCreatePipelineLayout(*m_ContextResources.device, &layoutDesc);

    WGPUBindGroupDescriptor bindGroupDescriptor {
        .layout = bindGroupLayout,
        .entryCount = bindGroups.size(),
        .entries = bindGroups.data(),
    };

    m_BindGroup = wgpuDeviceCreateBindGroup(*m_ContextResources.device, &bindGroupDescriptor);

    vertexBufferLayout = {
        .arrayStride = 4 * sizeof(float),
        .stepMode = WGPUVertexStepMode_Vertex,
        .attributeCount = attributes.size(),
        .attributes = attributes.data(),
    };

    shaderDescriptor = {
        .chain = {
            .next = nullptr,
            .sType = WGPUSType_ShaderModuleWGSLDescriptor
        },
        .code = shader.c_str()
    };

    shaderModuleDescriptor = {
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
        .layout = pipelineLayout,
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

    wgpuRenderPassEncoderSetBindGroup(*encoder, 0, m_BindGroup, 0, nullptr);

    wgpuRenderPassEncoderDrawIndexed(*encoder, FRAME_INDICES.size(), 1,0, 0, 0);
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

void FrameRenderer::ResizeAccumulatorTexture(const int, const int) {

}

void FrameRenderer::CopyAccumulatorToTexture(uint8_t *) {

}
