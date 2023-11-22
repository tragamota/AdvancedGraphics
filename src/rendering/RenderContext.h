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
    VkInstance m_Instance = VK_NULL_HANDLE;
    VkDevice m_Device = VK_NULL_HANDLE;

    VkQueue m_GraphicsQueue = VK_NULL_HANDLE;
    VkQueue m_PresentationQueue = VK_NULL_HANDLE;

    VkSurfaceKHR m_Surface = VK_NULL_HANDLE;
    VkSwapchainKHR m_SwapChain = VK_NULL_HANDLE;

    std::vector<VkImage> m_SwapChainImages;
    std::vector<VkImageView> m_SwapChainImageViews;

    VkSurfaceFormatKHR m_SurfaceFormat;
    VkPresentModeKHR m_PresentMode;
    VkExtent2D m_SurfaceExtent;

    RenderDevice* m_PhysicalDevice;
    RenderDeviceManager m_DeviceManager;

    void CreateInstance();
    void InitWindowSurface(GLFWwindow*);
    void InitDevices();
    void InitSwapChain();
    void InitImageViews();
    ImGui_ImplVulkan_InitInfo BuildImguiInfoStruct();
};

#endif //ADVANCEDGRAPHICS_RENDERCONTEXT_H
