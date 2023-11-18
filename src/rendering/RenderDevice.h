//
// Created by Ian on 17-11-2023.
//

#ifndef ADVANCEDGRAPHICS_RENDERDEVICE_H
#define ADVANCEDGRAPHICS_RENDERDEVICE_H

#include <vulkan/vulkan.h>
#include <string>

class RenderDevice {
    RenderDevice(VkPhysicalDevice);
    RenderDevice(RenderDevice&) = delete;

private:
    std::string deviceName;
    bool isDedicatedGpu;

};


#endif //ADVANCEDGRAPHICS_RENDERDEVICE_H
