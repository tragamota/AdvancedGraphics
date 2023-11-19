//
// Created by Ian on 17-11-2023.
//

#ifndef ADVANCEDGRAPHICS_RENDERCONTEXT_H
#define ADVANCEDGRAPHICS_RENDERCONTEXT_H

#include <vulkan/vulkan.h>
#include <backends/imgui_impl_vulkan.h>

#include "../core/Window.h"
#include "RenderDeviceManager.h"

class RenderContext {
public:
    void Init(std::shared_ptr<Window> window);
    void Destroy();
private:
    VkInstance m_Instance;
    VkDevice m_Device;

    VkQueue m_GraphicsQueue;
    VkQueue m_PresentationQueue;

    VkSurfaceKHR m_Surface;

    RenderDevice* m_PhysicalDevice;
    RenderDeviceManager m_DeviceManager;

    void CreateInstance();
    void InitWindowSurface(GLFWwindow*);
    void InitDevices();
    ImGui_ImplVulkan_InitInfo BuildImguiInfoStruct();
};

#endif //ADVANCEDGRAPHICS_RENDERCONTEXT_H
