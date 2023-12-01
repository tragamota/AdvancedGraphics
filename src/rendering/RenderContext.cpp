//
// Created by Ian on 17-11-2023.
//

#include <iostream>
#include <stdexcept>

#include "RenderContext.h"
#include "RenderDeviceManager.h"

#include <webgpu/webgpu_glfw.h>

void RenderContext::Init(std::shared_ptr<Window> window) {
    CreateInstance();
    InitWindowSurface(window->GetGLFWWindow());
    InitAdapter();
    InitDevices();
    InitSwapChain();
    InitImageViews();
}

void RenderContext::Destroy() {

}

void RenderContext::CreateInstance() {
    m_Instance = wgpu::CreateInstance();
}

void RenderContext::InitWindowSurface(GLFWwindow *window) {
    m_Surface = wgpu::glfw::CreateSurfaceForWindow(m_Instance, window);
}

void RenderContext::InitAdapter() {
    auto adapterOptions = wgpu::RequestAdapterOptions {
        .compatibleSurface = m_Surface,
        .powerPreference = wgpu::PowerPreference::HighPerformance,
    };

    wgpu::RequestAdapterCallback callback = [](WGPURequestAdapterStatus status, WGPUAdapter adapter, const char * message, void * ptr) {
        auto context = reinterpret_cast<RenderContext*>(ptr);

        if(static_cast<wgpu::RequestAdapterStatus>(status) == wgpu::RequestAdapterStatus::Success) {
            context->m_Adapter = static_cast<wgpu::Adapter>(adapter);
        }
        else {
            std::cout << message << std::endl;
        }
    };

    m_Instance.RequestAdapter(&adapterOptions, callback, this);

    if(m_Adapter == nullptr) {
        throw std::runtime_error("Adapter is not created");
    }
}



void RenderContext::InitDevices() {

}


void RenderContext::InitSwapChain() {

}

void RenderContext::InitImageViews() {

}



