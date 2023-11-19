//
// Created by Ian on 17-11-2023.
//

#ifndef ADVANCEDGRAPHICS_RENDERDEVICEMANAGER_H
#define ADVANCEDGRAPHICS_RENDERDEVICEMANAGER_H

#include <vector>
#include <memory>

#include <vulkan/vulkan.h>

#include "RenderDevice.h"

class RenderDeviceManager {
public:
    RenderDeviceManager() = default;
    RenderDeviceManager(RenderDeviceManager &other) = delete;
    ~RenderDeviceManager() = default;

    void Init(const VkInstance*);
//    std::vector<RenderDevice> AllRenderDevices();
    RenderDevice* PrimaryDevice();
private:
    bool checkDeviceSuitability(VkPhysicalDevice device);

    std::vector<RenderDevice> m_devices;
};

#endif //ADVANCEDGRAPHICS_RENDERDEVICEMANAGER_H
