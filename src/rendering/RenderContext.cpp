//
// Created by Ian on 17-11-2023.
//

#include <stdexcept>
#include <set>
#include "RenderContext.h"
#include "RenderDeviceManager.h"

void RenderContext::Init(std::shared_ptr<Window> window) {
    CreateInstance();
    InitWindowSurface(window->GetGLFWWindow());
    InitDevices();
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

    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Advanced Graphics";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_2;

    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    createInfo.enabledLayerCount = 0;
    createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
    createInfo.enabledExtensionCount = (uint32_t) requiredExtensions.size();;
    createInfo.ppEnabledExtensionNames = requiredExtensions.data();

    if (vkCreateInstance(&createInfo, nullptr, &m_Instance) != VK_SUCCESS) {
        throw std::runtime_error("failed to create instance!");
    }
}

void RenderContext::Destroy() {
    vkDestroyDevice(m_Device, nullptr);
    vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
    vkDestroyInstance(m_Instance, nullptr);
}

void RenderContext::InitDevices() {
    m_DeviceManager.Init(&m_Instance);
    m_PhysicalDevice = m_DeviceManager.PrimaryDevice();

    float queuePriority = 1.0f;

    std::set<uint32_t> uniqueQueueFamilies = {std::get<0>(m_PhysicalDevice->graphicsQueueIndex.value()),
                                              m_PhysicalDevice->surfaceSupportIndex.value()};
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos(uniqueQueueFamilies.size());

    for (uint32_t queueFamily : uniqueQueueFamilies) {
        VkDeviceQueueCreateInfo queueCreateInfo{};

        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = std::get<0>(m_PhysicalDevice->graphicsQueueIndex.value());
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;

        queueCreateInfos.emplace_back(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures{};

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.pEnabledFeatures = &deviceFeatures;
    createInfo.queueCreateInfoCount = 1;
    createInfo.enabledExtensionCount = 0;
    createInfo.enabledLayerCount = 0;

    if (vkCreateDevice(m_PhysicalDevice->GetPhysicalDevice(), &createInfo, nullptr, &m_Device) != VK_SUCCESS) {
        throw std::runtime_error("failed to create logical device!");
    }

    vkGetDeviceQueue(m_Device, std::get<0>(m_PhysicalDevice->graphicsQueueIndex.value()), 0, &m_GraphicsQueue);
    vkGetDeviceQueue(m_Device, m_PhysicalDevice->surfaceSupportIndex.value() , 0, &m_PresentationQueue);
}

void RenderContext::InitWindowSurface(GLFWwindow *window) {
    glfwCreateWindowSurface(m_Instance, window, nullptr, &m_Surface);
}

ImGui_ImplVulkan_InitInfo RenderContext::BuildImguiInfoStruct() {
    return {};
}

