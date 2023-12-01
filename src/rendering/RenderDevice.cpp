//
// Created by Ian on 17-11-2023.
//

#include "RenderDevice.h"

RenderDevice::RenderDevice() {

}

std::string &RenderDevice::GetDeviceName() {
    return m_DeviceName;
}

//VkPhysicalDevice &RenderDevice::GetPhysicalDevice() {
//    return m_Device;
//}

bool RenderDevice::IsDedicatedGpu() const {
    return m_IsDedicatedGpu;
}

void RenderDevice::ExtractDeviceInfo() {
    QuerySwapChainSupport();
}

void RenderDevice::QuerySwapChainSupport() {

}

