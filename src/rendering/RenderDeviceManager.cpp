//
// Created by Ian on 17-11-2023.
//

#include <stdexcept>
#include <iostream>

#ifdef 

#include <glfw/glfw3native.h.h>

#include "RenderDeviceManager.h"

RenderDeviceManager* RenderDeviceManager::singleton_ = nullptr;

void RenderDeviceManager::QueryAllSuitableDevices(const VkInstance* renderInstance) {
    uint32_t deviceCount = 0;
    VkPhysicalDevice selected_device;

    vkEnumeratePhysicalDevices(*renderInstance, &deviceCount, nullptr);

    std::vector<VkPhysicalDevice> devices(deviceCount);

    if (deviceCount == 0) {
        throw std::runtime_error("failed to find GPUs with Vulkan support!");
    }


    vkEnumeratePhysicalDevices(*renderInstance, &deviceCount, devices.data());

    for (const auto& device : devices) {
        if(checkDeviceSuitability(device)) {
            selected_device = device;
        }
    }
}

//std::vector<RenderDevice> RenderDeviceManager::AllRenderDevices() const {
//    return nullptr;
//}

//std::shared_ptr<RenderDevice> RenderDeviceManager::PrimaryDevice() const {
//    return nullptr;
//}

bool RenderDeviceManager::checkDeviceSuitability(VkPhysicalDevice device) {
    VkPhysicalDeviceProperties deviceProperties;
    VkPhysicalDeviceFeatures deviceFeatures;

    vkGetPhysicalDeviceProperties(device, &deviceProperties);
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

    return true;
}
