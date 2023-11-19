//
// Created by Ian on 17-11-2023.
//

#include "RenderDevice.h"

RenderDevice::RenderDevice(VkPhysicalDevice device) {
    m_Device = device;
}

std::string &RenderDevice::GetDeviceName() {
    return m_DeviceName;
}

VkPhysicalDevice &RenderDevice::GetPhysicalDevice() {
    return m_Device;
}

bool RenderDevice::IsDedicatedGpu() const {
    return m_IsDedicatedGpu;
}

void RenderDevice::ExtractDeviceInfo(const VkSurfaceKHR *surface) {
    uint32_t queueFamilyCount = 0;
    std::vector<VkQueueFamilyProperties> queueFamilies;

    VkPhysicalDeviceProperties deviceProperties;

    vkGetPhysicalDeviceProperties(m_Device, &deviceProperties);
    vkGetPhysicalDeviceQueueFamilyProperties(m_Device, &queueFamilyCount, nullptr);
    queueFamilies.resize(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(m_Device, &queueFamilyCount, queueFamilies.data());

    m_DeviceName = deviceProperties.deviceName;
    m_IsDedicatedGpu = deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;

    for (int i = 0; i < queueFamilyCount; i++) {
        VkBool32 hasSurfaceSupport = false;
        VkQueueFamilyProperties properties = queueFamilies[i];

        vkGetPhysicalDeviceSurfaceSupportKHR(m_Device, i, *surface, &hasSurfaceSupport);

        bool hasGraphicsBit = (properties.queueFlags & VK_QUEUE_GRAPHICS_BIT);

        if (hasGraphicsBit && hasSurfaceSupport) {
            graphicsQueueIndex = {i, properties.queueCount};
            surfaceSupportIndex = i;
            break;
        }

        if (hasGraphicsBit) {
            if (!graphicsQueueIndex.has_value()) {
                graphicsQueueIndex = {i, properties.queueCount};
            }
        }

        if (hasSurfaceSupport) {
            if (!surfaceSupportIndex.has_value()) {
                surfaceSupportIndex = i;
            }
        }
    }

    QuerySwapChainSupport(surface);
}

void RenderDevice::QuerySwapChainSupport(const VkSurfaceKHR* surface) {
    uint32_t presentModeCount, formatCount;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_Device, *surface, &surfaceCapabilities);
    vkGetPhysicalDeviceSurfaceFormatsKHR(m_Device, *surface, &formatCount, nullptr);
    vkGetPhysicalDeviceSurfacePresentModesKHR(m_Device, *surface, &presentModeCount, nullptr);

    if (formatCount != 0) {
        surfaceFormats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(m_Device, *surface, &formatCount, surfaceFormats.data());
    }

    if (presentModeCount != 0) {
        presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(m_Device, *surface, &presentModeCount, presentModes.data());
    }
}

