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
    CreateInstance();
    InitWindowSurface(window->GetGLFWWindow());
    InitAdapter();
    InitDevices();
    InitSwapChain(window->GetWindowFrameBuffer());
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

    auto onDeviceError = [](WGPUErrorType type, char const* message, void*) {
        std::cout << "Uncaptured device error: type " << type;

        if (message)
            std::cout << " (" << message << ")";

        std::cout << std::endl;
    };

    wgpuDeviceSetUncapturedErrorCallback(m_Device, onDeviceError, nullptr);
}


void RenderContext::InitSwapChain(WindowFrameSize frameSize) {
    WGPUSwapChainDescriptor swapChainDesc = {
        .usage = WGPUTextureUsage_RenderAttachment,
        .format = m_PreferredTextureFormat,
        .width = static_cast<uint32_t>(frameSize.width),
        .height = static_cast<uint32_t>(frameSize.height),
        .presentMode = WGPUPresentMode_Fifo
    };

    m_SwapChain = wgpuDeviceCreateSwapChain(m_Device, m_Surface, &swapChainDesc);
}

void RenderContext::Present() {
    wgpuSwapChainPresent(m_SwapChain);
    wgpuTextureViewRelease(m_TextureView);
}

const WGPURenderPassEncoder* RenderContext::GetRenderPass() {
    m_TextureView = wgpuSwapChainGetCurrentTextureView(m_SwapChain);

    WGPUCommandEncoderDescriptor commandEncoderDescriptor { };

    WGPURenderPassColorAttachment attachment {
        .view = m_TextureView,
        .loadOp = WGPULoadOp_Clear,
        .storeOp = WGPUStoreOp_Store,
        .clearValue = WGPUColor {0.05f, 0.05f, 0.05f, 0.5f}
    };

    WGPURenderPassDescriptor renderPassDesc {
        .colorAttachmentCount = 1,
        .colorAttachments = &attachment
    };

    m_CurrentCommandEncoder = wgpuDeviceCreateCommandEncoder(m_Device, &commandEncoderDescriptor);
    m_CurrentRenderPassEncoder = wgpuCommandEncoderBeginRenderPass(m_CurrentCommandEncoder, &renderPassDesc);

    return &m_CurrentRenderPassEncoder;
}

void RenderContext::SubmitCommandBuffer() {
    WGPUCommandBufferDescriptor commandBufferDescriptor {};
    WGPUCommandBuffer commandBuffer {};

    wgpuRenderPassEncoderEnd(m_CurrentRenderPassEncoder);
    commandBuffer = wgpuCommandEncoderFinish(m_CurrentCommandEncoder, &commandBufferDescriptor);

    wgpuQueueSubmit(m_Queue, 1, &commandBuffer);

    wgpuCommandBufferRelease(commandBuffer);
    wgpuCommandEncoderRelease(m_CurrentCommandEncoder);
    wgpuRenderPassEncoderRelease(m_CurrentRenderPassEncoder);
}

bool RenderContext::HasError() const {
    return m_DawnError;
}

const std::string& RenderContext::ErrorMessage() const {
    return m_DawnErrorMessage;
}

void RenderContext::ResizeSwapChain(WindowFrameSize frameSize) {
    wgpuSwapChainRelease(m_SwapChain);

    InitSwapChain(frameSize);
}

ContextResources RenderContext::GetContextResources() {
    return ContextResources {
        .device = &m_Device,
        .queue = &m_Queue,
        .adapter = &m_Adapter,
        .surfaceFormat = m_PreferredTextureFormat
    };
}