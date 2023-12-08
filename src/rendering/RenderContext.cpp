//
// Created by Ian on 17-11-2023.
//

#include <iostream>
#include <stdexcept>
#include <fstream>
#include <sstream>

#include "RenderContext.h"

#include "../platform/GlfwSurfaceExtension.h"

void RenderContext::Init(const std::shared_ptr<Window>& window) {
    int width, height;
    glfwGetFramebufferSize(window->GetGLFWWindow(), &width, &height);

    CreateInstance();
    InitWindowSurface(window->GetGLFWWindow());
    InitAdapter();
    InitDevices();
    InitSwapChain(width, height);
}

void RenderContext::Destroy() {
    wgpuCommandEncoderRelease(m_CurrentCommandEncoder);
    wgpuRenderPassEncoderRelease(m_CurrentRenderPassEncoder);

    wgpuSwapChainRelease(m_SwapChain);
    wgpuSurfaceRelease(m_Surface);
    wgpuDeviceRelease(m_Device);
    wgpuAdapterRelease(m_Adapter);
    wgpuInstanceRelease(m_Instance);
}

void RenderContext::RegisterInterface(const std::shared_ptr<Window>& window, Interface *interface) {
    interface->Init(window, &m_Device, WGPUTextureFormat_BGRA8Unorm);
}

void RenderContext::CreateInstance() {
    WGPUInstanceDescriptor desc = {};
    desc.nextInChain = nullptr;

    m_Instance = wgpuCreateInstance(&desc);

    if (!m_Instance) {
        std::cerr << "Could not initialize WebGPU!" << std::endl;
    }
}

void RenderContext::InitWindowSurface(GLFWwindow *window) {
    m_Surface = glfwGetWGPUSurface(m_Instance, window);

    if (m_Surface == nullptr) {
        throw std::runtime_error("Surface creation failed");
    }
}

void RenderContext::InitAdapter() {
    auto adapterOptions = WGPURequestAdapterOptions{
            .compatibleSurface = m_Surface,
            .powerPreference = WGPUPowerPreference_HighPerformance,
    };

    WGPURequestAdapterCallback callback = [](WGPURequestAdapterStatus status, WGPUAdapter adapter,
                                             const char *message, void *ptr) {
        auto context = reinterpret_cast<RenderContext *>(ptr);

        if (status == WGPURequestAdapterStatus_Success) {
            context->m_Adapter = adapter;
            return;
        }

        if (status == WGPURequestAdapterStatus_Error ||
            status == WGPURequestAdapterStatus_Unknown) {
            context->m_DawnError = true;
            context->m_DawnErrorMessage = std::string(message);
        }
    };

    wgpuInstanceRequestAdapter(m_Instance, &adapterOptions, callback, this);

    if (m_Adapter == nullptr) {
        throw std::runtime_error("Adapter is not created");
    }
}

void RenderContext::InitDevices() {
    WGPUDeviceDescriptor descriptor = {
            .requiredFeatureCount = 0,
            .requiredFeatures = nullptr,
            .deviceLostCallback = nullptr,
            .deviceLostUserdata = nullptr
    };

    WGPURequestDeviceCallback callback = [](WGPURequestDeviceStatus status, WGPUDevice device, char const *message,
                                              void *ptr) {
        auto context = reinterpret_cast<RenderContext *>(ptr);

        if (status == WGPURequestDeviceStatus_Success) {
            context->m_Device = device;
            return;
        }

        if (status == WGPURequestDeviceStatus_Error) {
            context->m_DawnError = true;
            context->m_DawnErrorMessage = std::string(message);
        }
    };

    wgpuAdapterRequestDevice(m_Adapter, &descriptor, callback, this);

    if (m_Adapter == nullptr) {
        throw std::runtime_error("Adapter is not created");
    }

    m_Queue = wgpuDeviceGetQueue(m_Device);

    if (m_Queue == nullptr) {
        throw std::runtime_error("Requesting Queue failed");
    }

    auto onDeviceError = [](WGPUErrorType type, char const* message, void* /* pUserData */) {
        std::cout << "Uncaptured device error: type " << type;

        if (message)
            std::cout << " (" << message << ")";

        std::cout << std::endl;
    };

    wgpuDeviceSetUncapturedErrorCallback(m_Device, onDeviceError, nullptr);
}


void RenderContext::InitSwapChain(int width, int height) {
    WGPUSwapChainDescriptor swapChainDesc = {
        .usage = WGPUTextureUsage_RenderAttachment,
        .format = WGPUTextureFormat_BGRA8Unorm,
        .width = static_cast<uint32_t>(width),
        .height = static_cast<uint32_t>(height),
        .presentMode = WGPUPresentMode_Fifo
    };

    m_SwapChain = wgpuDeviceCreateSwapChain(m_Device, m_Surface, &swapChainDesc);
}

void RenderContext::Present() {
    wgpuSwapChainPresent(m_SwapChain);
    wgpuTextureViewRelease(m_TextureView);
}

WGPURenderPassEncoder RenderContext::GetRenderPass() {
    m_TextureView = wgpuSwapChainGetCurrentTextureView(m_SwapChain);

    if(m_CurrentCommandEncoder  && m_CurrentCommandEncoder) {
        wgpuCommandEncoderRelease(m_CurrentCommandEncoder);
        wgpuRenderPassEncoderRelease(m_CurrentRenderPassEncoder);
    }

    WGPURenderPassColorAttachment attachment{
            .view = m_TextureView,
            .loadOp = WGPULoadOp_Clear,
            .storeOp = WGPUStoreOp_Store,
            .clearValue = WGPUColor {1.0f, 1.0f, 1.0f, 0.0f}
    };

    WGPURenderPassDescriptor renderPassDesc {
            .colorAttachmentCount = 1,
            .colorAttachments = &attachment
    };

    WGPUCommandEncoderDescriptor commandEncoderDescriptor = {};

    m_CurrentCommandEncoder = wgpuDeviceCreateCommandEncoder(m_Device, &commandEncoderDescriptor);
    m_CurrentRenderPassEncoder = wgpuCommandEncoderBeginRenderPass(m_CurrentCommandEncoder, &renderPassDesc);

    return m_CurrentRenderPassEncoder;
}

std::string RenderContext::ReadShaderCode(const char * filePath) {
    std::stringstream buffer;
    std::ifstream inputFile(filePath);

    if (!inputFile.is_open()) {
        throw std::runtime_error("Shader file doesn't exist");
    }

    buffer << inputFile.rdbuf();

    inputFile.close();

    return buffer.str();
}

WGPURenderPipeline RenderContext::CreateRenderPipeline(const char *shaderPath) {
    auto shaderCode = ReadShaderCode(shaderPath);

    WGPUColorTargetState colorTargetState {
        .format = WGPUTextureFormat_RGBA8Unorm,
    };

    WGPUShaderModuleWGSLDescriptor shader_desc {
        .code = shaderCode.c_str()
    };

    WGPUShaderModuleDescriptor shaderModuleDescriptor {};
    WGPUShaderModule shaderModule = wgpuDeviceCreateShaderModule(m_Device, &shaderModuleDescriptor);

    WGPUVertexState vertexState {
        .module = shaderModule,
        .entryPoint = "vert_main",
        .constantCount = 0,
        .constants = nullptr,
        .bufferCount = 2,
        .buffers = nullptr
    };

    WGPUFragmentState fragmentState {
        .module = shaderModule,
        .entryPoint = "frag_main",
        .constantCount = 0,
        .constants = nullptr,
        .targetCount = 1,
        .targets = &colorTargetState
    };

    WGPURenderPipelineDescriptor descriptor{
        .vertex = vertexState,
        .depthStencil = nullptr,
        .fragment = &fragmentState,

    };

    descriptor.primitive.topology = WGPUPrimitiveTopology_TriangleList;
    descriptor.primitive.frontFace = WGPUFrontFace_CCW;
    descriptor.primitive.cullMode = WGPUCullMode_None;
    descriptor.primitive.stripIndexFormat = WGPUIndexFormat_Uint32;

    descriptor.multisample = WGPUMultisampleState {
        .count = 1,
        .alphaToCoverageEnabled = false
    };

    return wgpuDeviceCreateRenderPipeline(m_Device, &descriptor);
}

void RenderContext::SubmitCommandBuffer(int commandCount) {
    WGPUCommandBufferDescriptor descriptor {};

    auto commandBuffer = wgpuCommandEncoderFinish(m_CurrentCommandEncoder, &descriptor);

    wgpuQueueSubmit(m_Queue, 1, &commandBuffer);
    wgpuCommandBufferRelease(commandBuffer);
}

bool RenderContext::HasError() const {
    return m_DawnError;
}

const std::string& RenderContext::ErrorMessage() const {
    return m_DawnErrorMessage;
}

void RenderContext::ResizeSwapChain(int width, int height) {
    wgpuSwapChainRelease(m_SwapChain);

    InitSwapChain(width, height);
}
