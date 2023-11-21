//
// Created by Ian on 17-11-2023.
//

#include <iostream>
#include <stdexcept>
#include <set>

#include "RenderContext.h"
#include "RenderDeviceManager.h"

#ifdef __APPLE__
#include <vulkan/vulkan_beta.h>
#endif

const std::vector<const char *> validationLayers = {
        "VK_LAYER_KHRONOS_validation"
};

const std::vector<const char *> deviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

void RenderContext::Init(std::shared_ptr<Window> window) {
    CreateInstance();
    InitWindowSurface(window->GetGLFWWindow());
    InitDevices();
    InitSwapChain();
}

void RenderContext::CreateInstance() {
    VkApplicationInfo appInfo{};
    VkInstanceCreateInfo createInfo{};

    std::vector<const char *> requiredExtensions;

    uint32_t glfwExtensionCount = 0;
    const char **glfwExtensionNames = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    for (uint32_t i = 0; i < glfwExtensionCount; i++) {
        requiredExtensions.emplace_back(glfwExtensionNames[i]);
    }

    requiredExtensions.emplace_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
    requiredExtensions.emplace_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);

    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Advanced Graphics";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_2;

    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    createInfo.enabledLayerCount = validationLayers.size();
    createInfo.ppEnabledLayerNames = validationLayers.data();
    createInfo.flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
    createInfo.enabledExtensionCount = (uint32_t) requiredExtensions.size();
    createInfo.ppEnabledExtensionNames = requiredExtensions.data();

    if (vkCreateInstance(&createInfo, nullptr, &m_Instance) != VK_SUCCESS) {
        throw std::runtime_error("failed to create instance!");
    }
}

void RenderContext::Destroy() {
    vkDestroySwapchainKHR(m_Device, m_SwapChain, nullptr);
    vkDestroyDevice(m_Device, nullptr);
    vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
    vkDestroyInstance(m_Instance, nullptr);
}

void RenderContext::InitDevices() {
    m_DeviceManager.Init(&m_Instance, &m_Surface);
    m_PhysicalDevice = m_DeviceManager.PrimaryDevice();

    float queuePriority = 1.0f;

    std::set<uint32_t> uniqueQueueFamilies = {std::get<0>(m_PhysicalDevice->graphicsQueueIndex.value()),
                                              m_PhysicalDevice->surfaceSupportIndex.value()};
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

    for (uint32_t queueFamily: uniqueQueueFamilies) {
        VkDeviceQueueCreateInfo queueCreateInfo{};

        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfo.pNext = nullptr;

        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures{};

#ifdef  __APPLE__
    deviceExtensions.emplace_back(VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME);
#endif

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.pEnabledFeatures = &deviceFeatures;
    createInfo.queueCreateInfoCount = queueCreateInfos.size();
    createInfo.enabledExtensionCount = 0;
    createInfo.enabledLayerCount = 0;
    createInfo.ppEnabledExtensionNames = deviceExtensions.data();
    createInfo.enabledExtensionCount = deviceExtensions.size();

    if (vkCreateDevice(m_PhysicalDevice->GetPhysicalDevice(), &createInfo, nullptr, &m_Device) != VK_SUCCESS) {
        throw std::runtime_error("failed to create logical device!");
    }

    vkGetDeviceQueue(m_Device, std::get<0>(m_PhysicalDevice->graphicsQueueIndex.value()), 0, &m_GraphicsQueue);
    vkGetDeviceQueue(m_Device, m_PhysicalDevice->surfaceSupportIndex.value(), 0, &m_PresentationQueue);
}

void RenderContext::InitWindowSurface(GLFWwindow *window) {
    if (glfwCreateWindowSurface(m_Instance, window, nullptr, &m_Surface) != VK_SUCCESS) {
        throw std::runtime_error("failed to create render surface!");
    }
}

void RenderContext::InitSwapChain() {
    m_SurfaceFormat = m_PhysicalDevice->surfaceFormats.front();
    m_PresentMode = VK_PRESENT_MODE_FIFO_KHR;

    for (const auto& availableFormat : m_PhysicalDevice->surfaceFormats) {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            m_SurfaceFormat = availableFormat;
            break;
        }
    }

    for (const auto& availablePresentMode : m_PhysicalDevice->presentModes) {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
             m_PresentMode = availablePresentMode;
             break;
        }
    }

    m_SurfaceExtent = m_PhysicalDevice->surfaceCapabilities.currentExtent;

    std::cout << m_PhysicalDevice->surfaceCapabilities.currentExtent.width << std::endl;
    std::cout << m_PhysicalDevice->surfaceCapabilities.currentExtent.height << std::endl;

    uint32_t frameCount = m_PhysicalDevice->surfaceCapabilities.maxImageCount;

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = m_Surface;
    createInfo.minImageCount = frameCount;
    createInfo.imageFormat = m_SurfaceFormat.format;
    createInfo.imageColorSpace = m_SurfaceFormat.colorSpace;
    createInfo.imageExtent = m_SurfaceExtent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    createInfo.preTransform = m_PhysicalDevice->surfaceCapabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = m_PresentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = m_SwapChain;

    uint32_t queueFamilyIndices[] = {std::get<0>(m_PhysicalDevice->graphicsQueueIndex.value()),m_PhysicalDevice->surfaceSupportIndex.value()};

    if (std::get<0>(m_PhysicalDevice->graphicsQueueIndex.value()) != m_PhysicalDevice->surfaceSupportIndex.value()) {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    } else {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0; // Optional
        createInfo.pQueueFamilyIndices = nullptr; // Optional
    }

    if (vkCreateSwapchainKHR(m_Device, &createInfo, nullptr, &m_SwapChain) != VK_SUCCESS) {
        throw std::runtime_error("failed to create swap chain!");
    }

//    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
//        return capabilities.currentExtent;
//    } else {
//        int width, height;
//        glfwGetFramebufferSize(window, &width, &height);
//
//        VkExtent2D actualExtent = {
//                static_cast<uint32_t>(width),
//                static_cast<uint32_t>(height)
//        };
//
//        actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width,
//                                        capabilities.maxImageExtent.width);
//        actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height,
//                                         capabilities.maxImageExtent.height);
//
//        return actualExtent;
//    }
}


ImGui_ImplVulkan_InitInfo RenderContext::BuildImguiInfoStruct() {
    return {};
}

