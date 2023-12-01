//
// Created by Ian on 17-11-2023.
//

#ifndef ADVANCEDGRAPHICS_RENDERCONTEXT_H
#define ADVANCEDGRAPHICS_RENDERCONTEXT_H

//#include <webgpu/webgpu_glfw.h>
#include <webgpu/webgpu_cpp.h>

#include "../core/Window.h"

#include "RenderDeviceManager.h"

class RenderContext {
public:
    void Init(std::shared_ptr<Window> window);
    void Destroy();
private:
    wgpu::Instance m_Instance;
    wgpu::Surface m_Surface;
    wgpu::Adapter m_Adapter;

    RenderDevice* m_PhysicalDevice;
    RenderDeviceManager m_DeviceManager;

    inline void CreateInstance();
    inline void InitWindowSurface(GLFWwindow*);
    inline void InitAdapter();
    inline void InitDevices();
    inline void InitSwapChain();
    inline void InitImageViews();
};

#endif //ADVANCEDGRAPHICS_RENDERCONTEXT_H
