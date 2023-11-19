//
// Created by Ian on 17-11-2023.
//

#include <stdexcept>
#include <iostream>

//#ifdef
//
//#include <glfw/glfw3native.h.h>

#include "RenderDeviceManager.h"

void RenderDeviceManager::Init(const VkInstance* renderInstance, const VkSurfaceKHR* surface) {
    uint32_t deviceCount = 0;
    std::vector<VkPhysicalDevice> physicalDevices;

    vkEnumeratePhysicalDevices(*renderInstance, &deviceCount, nullptr);
    physicalDevices.resize(deviceCount);
    vkEnumeratePhysicalDevices(*renderInstance, &deviceCount, physicalDevices.data());

    if (deviceCount == 0) {
        throw std::runtime_error("failed to find GPUs with Vulkan support!");
    }

    for (int i = 0; i < deviceCount; i++) {
        auto device = physicalDevices[i];
        auto renderDevice = RenderDevice(device);

        renderDevice.ExtractDeviceInfo(surface);

        m_devices.emplace_back(renderDevice);
    }
}

bool RenderDeviceManager::checkDeviceSuitability(VkPhysicalDevice device) {
    VkPhysicalDeviceProperties deviceProperties;
    VkPhysicalDeviceFeatures deviceFeatures;

    vkGetPhysicalDeviceProperties(device, &deviceProperties);
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

    return true;
}

RenderDevice* RenderDeviceManager::PrimaryDevice() {
    return &m_devices.front();
}


