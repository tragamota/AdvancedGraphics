//
// Created by Ian on 17-11-2023.
//

#ifndef ADVANCEDGRAPHICS_RENDERCONTEXT_H
#define ADVANCEDGRAPHICS_RENDERCONTEXT_H

#include <vulkan/vulkan.h>
#include "../core/Window.h"

class RenderContext {
public:
    void Init();
    void Destroy();

private:
    VkInstance instance;

    void createInstance();

    void FetchPhysicalDevice();
};

#endif //ADVANCEDGRAPHICS_RENDERCONTEXT_H
