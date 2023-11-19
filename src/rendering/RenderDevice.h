//
// Created by Ian on 17-11-2023.
//

#ifndef ADVANCEDGRAPHICS_RENDERDEVICE_H
#define ADVANCEDGRAPHICS_RENDERDEVICE_H

#include <vulkan/vulkan.h>
#include <string>
#include <vector>
#include <optional>

class RenderDevice {
public:
    RenderDevice(VkPhysicalDevice);
    RenderDevice(const RenderDevice&) = default;
    RenderDevice(RenderDevice&& other) = default;
    ~RenderDevice() = default;

    std::optional<std::tuple<uint32_t, uint32_t>> graphicsQueueIndex;
    std::optional<uint32_t> surfaceSupportIndex;

    VkSurfaceCapabilitiesKHR surfaceCapabilities;
    std::vector<VkSurfaceFormatKHR> surfaceFormats;
    std::vector<VkPresentModeKHR> presentModes;

    void ExtractDeviceInfo(const VkSurfaceKHR*);

    VkPhysicalDevice& GetPhysicalDevice();
    std::string& GetDeviceName();
    [[nodiscard]] bool IsDedicatedGpu() const;
private:
    VkPhysicalDevice m_Device;

    bool m_IsDedicatedGpu;
    std::string m_DeviceName;

    void QuerySwapChainSupport(const VkSurfaceKHR *);
};


#endif //ADVANCEDGRAPHICS_RENDERDEVICE_H
