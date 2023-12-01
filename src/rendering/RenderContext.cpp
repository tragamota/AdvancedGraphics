//
// Created by Ian on 17-11-2023.
//

#include <iostream>
#include <stdexcept>

#include "RenderContext.h"

#include "../misc/glfw3webgpu.h"

void RenderContext::Init(std::shared_ptr<Window> window) {
    CreateInstance();
    InitWindowSurface(window->GetGLFWWindow());
    InitAdapter();
    InitDevices();

    auto onDeviceError = [](WGPUErrorType type, char const* message, void* /* pUserData */) {
        std::cout << "Uncaptured device error: type " << type;
        if (message) std::cout << " (" << message << ")";
        std::cout << std::endl;
    };

    wgpuDeviceSetUncapturedErrorCallback(m_Device, onDeviceError, nullptr);

    InitSwapChain(window->GetGLFWWindow());
    InitImageViews();
}

void RenderContext::Destroy() {
    wgpuSwapChainRelease(m_SwapChain);
    wgpuSurfaceRelease(m_Surface);
    wgpuDeviceRelease(m_Device);
    wgpuAdapterRelease(m_Adapter);
    wgpuInstanceRelease(m_Instance);
}

void RenderContext::RegisterInterface(std::shared_ptr<Window> window, Interface *interface) {
    interface->Init(window, &m_Device, WGPUTextureFormat_RGBA8Unorm);
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
            context->m_DawnErrorMessage = message;
        }
    };

    wgpuInstanceRequestAdapter(m_Instance, &adapterOptions, callback, this);

    if (m_Adapter == nullptr) {
        throw std::runtime_error("Adapter is not created");
    }
}

void RenderContext::InitDevices() {
    WGPUSupportedLimits limits;
    WGPUAdapterProperties properties;
    WGPUDeviceDescriptor descriptor = {};

    WGPURequestDeviceCallback callback = [](WGPURequestDeviceStatus status, WGPUDevice device, char const *message,
                                              void *ptr) {
        auto context = reinterpret_cast<RenderContext *>(ptr);

        if (status == WGPURequestDeviceStatus_Success) {
            context->m_Device = device;
            return;
        }

        if (status == WGPURequestDeviceStatus_Error) {
            context->m_DawnError = true;
            context->m_DawnErrorMessage = message;
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
}


void RenderContext::InitSwapChain(GLFWwindow *window) {
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    WGPUSwapChainDescriptor swapChainDesc = {
            .usage = WGPUTextureUsage_RenderAttachment,
            .format = WGPUTextureFormat_BGRA8Unorm,
            .width = static_cast<uint32_t>(width),
            .height = static_cast<uint32_t>(height),
            .presentMode = WGPUPresentMode_Fifo
    };

    m_SwapChain = wgpuDeviceCreateSwapChain(m_Device, m_Surface, &swapChainDesc);
}

void RenderContext::InitImageViews() {

}

void RenderContext::Present() {
    wgpuSwapChainPresent(m_SwapChain);

    wgpuTextureViewRelease(m_TextureView);
}

WGPURenderPassEncoder RenderContext::GetRenderPass() {
    if(m_CurrentCommandEncoder) {
        wgpuCommandEncoderRelease(m_CurrentCommandEncoder);
        wgpuRenderPassEncoderRelease(m_CurrentRenderPassEncoder);
    }

    m_TextureView = wgpuSwapChainGetCurrentTextureView(m_SwapChain);

    WGPURenderPassColorAttachment attachment{
            .view = m_TextureView,
            .loadOp = WGPULoadOp_Clear,
            .storeOp = WGPUStoreOp_Store,
            .clearValue = WGPUColor(0.1f, 0.1f, 0.1f, 0.0f)
    };

    WGPURenderPassDescriptor renderPassDesc {
            .colorAttachmentCount = 1,
            .colorAttachments = &attachment
    };

    WGPUCommandEncoderDescriptor commandEncoderDescriptor = {
    };

    m_CurrentCommandEncoder = wgpuDeviceCreateCommandEncoder(m_Device, &commandEncoderDescriptor);
    m_CurrentRenderPassEncoder = wgpuCommandEncoderBeginRenderPass(m_CurrentCommandEncoder, &renderPassDesc);

    return m_CurrentRenderPassEncoder;
}

const char *RenderContext::ReadShaderCode(const char *) {
    return nullptr;
}

WGPURenderPipeline RenderContext::CreateRenderPipeline(const char *shaderPath) {
    auto shaderCode = ReadShaderCode(shaderPath);

    WGPUColorTargetState colorTargetState{
            .format = WGPUTextureFormat_RGBA8UnormSrgb
    };

    WGPUShaderModuleWGSLDescriptor wgslDesc {};
    wgslDesc.code = shaderCode;

    WGPUShaderModuleDescriptor shaderModuleDescriptor {};

    WGPUShaderModule shaderModule = wgpuDeviceCreateShaderModule(m_Device, &shaderModuleDescriptor);

    WGPUVertexState vertexState{
            .module = shaderModule,
            .entryPoint = "vert_main",
    };

    WGPUFragmentState fragmentState{
            .module = shaderModule,
            .entryPoint = "frag_main",
            .targetCount = 1,
            .targets = &colorTargetState
    };

    WGPURenderPipelineDescriptor descriptor{
            .vertex = vertexState,
            .fragment = &fragmentState
    };

    return wgpuDeviceCreateRenderPipeline(m_Device, &descriptor);
}

void RenderContext::SubmitCommandBuffer() {
    WGPUCommandBufferDescriptor descriptor {};

    auto commandBuffer = wgpuCommandEncoderFinish(m_CurrentCommandEncoder, &descriptor);

    wgpuQueueSubmit(m_Queue, 1, &commandBuffer);

    wgpuCommandBufferRelease(commandBuffer);
}






