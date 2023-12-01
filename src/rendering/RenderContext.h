//
// Created by Ian on 17-11-2023.
//

#ifndef ADVANCEDGRAPHICS_RENDERCONTEXT_H
#define ADVANCEDGRAPHICS_RENDERCONTEXT_H

#include <webgpu/webgpu.h>

#include "../core/Window.h"
#include "../Interface.h"

class RenderContext {
public:
    void Init(std::shared_ptr<Window> window);
    void Destroy();

    void RegisterInterface(std::shared_ptr<Window> window, Interface* interface);
    void Present();

    WGPURenderPassEncoder GetRenderPass();
    WGPURenderPipeline CreateRenderPipeline(const char* );

    void SubmitCommandBuffer();

private:
    bool m_DawnError = false;
    const char* m_DawnErrorMessage = nullptr;

    WGPUInstance m_Instance   = nullptr;
    WGPUSurface m_Surface     = nullptr;
    WGPUAdapter m_Adapter     = nullptr;
    WGPUDevice m_Device       = nullptr;
    WGPUQueue m_Queue         = nullptr;
    WGPUSwapChain m_SwapChain = nullptr;

    WGPUTextureView m_TextureView = nullptr;

    WGPUCommandEncoder m_CurrentCommandEncoder = nullptr;
    WGPURenderPassEncoder m_CurrentRenderPassEncoder = nullptr;

    inline void CreateInstance();
    inline void InitWindowSurface(GLFWwindow*);
    inline void InitAdapter();
    inline void InitDevices();
    inline void InitSwapChain(GLFWwindow*);
    inline void InitImageViews();

    const char* ReadShaderCode(const char*);
};

#endif //ADVANCEDGRAPHICS_RENDERCONTEXT_H
