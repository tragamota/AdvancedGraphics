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
private:
    RenderDeviceManager() = default;

    std::vector<RenderDevice> m_devices;

public:
    RenderDeviceManager(RenderDeviceManager &other) = delete;

    static inline RenderDeviceManager* GetInstance() {
        if(singleton_ == nullptr){
            singleton_ = new RenderDeviceManager();
        }

        return singleton_;
    }

    void QueryAllSuitableDevices(const VkInstance*);
//    std::vector<RenderDevice> AllRenderDevices() const;
//    std::shared_ptr<RenderDevice> PrimaryDevice() const;

    bool checkDeviceSuitability(VkPhysicalDevice device);

    void operator=(const RenderDeviceManager&) = delete;
protected:
    static RenderDeviceManager* singleton_;
};

#endif //ADVANCEDGRAPHICS_RENDERDEVICEMANAGER_H
