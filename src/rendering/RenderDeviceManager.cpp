//
// Created by Ian on 17-11-2023.
//

#include <stdexcept>
#include <iostream>

#include "RenderDeviceManager.h"

void RenderDeviceManager::Init() {

}

bool RenderDeviceManager::checkDeviceSuitability() {

    return true;
}

RenderDevice* RenderDeviceManager::PrimaryDevice() {
    return &m_devices.front();
}



