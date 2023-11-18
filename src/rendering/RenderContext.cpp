//
// Created by Ian on 17-11-2023.
//

#include <stdexcept>
#include "RenderContext.h"
#include "RenderDeviceManager.h"

void RenderContext::createInstance() {
    VkApplicationInfo appInfo {};
    VkInstanceCreateInfo createInfo {};

    std::vector<const char*> requiredExtensions;

    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensionNames = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    for(uint32_t i = 0; i < glfwExtensionCount; i++) {
        requiredExtensions.emplace_back(glfwExtensionNames[i]);
    }

    requiredExtensions.emplace_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);

    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Advanced Graphics";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_2;

    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    createInfo.enabledLayerCount = 0;
    createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
    createInfo.enabledExtensionCount = (uint32_t) requiredExtensions.size();;
    createInfo.ppEnabledExtensionNames = requiredExtensions.data();;

    if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
        throw std::runtime_error("failed to create instance!");
    }
}


void RenderContext::Init() {
    createInstance();
    FetchPhysicalDevice();
}

void RenderContext::Destroy() {
    vkDestroyInstance(instance, nullptr);
}

void RenderContext::FetchPhysicalDevice() {
    RenderDeviceManager::GetInstance()->QueryAllSuitableDevices(&instance);
}

