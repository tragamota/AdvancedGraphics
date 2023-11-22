//
// Created by Ian on 19-11-2023.
//

#ifndef ADVANCEDGRAPHICS_INTERFACE_H
#define ADVANCEDGRAPHICS_INTERFACE_H

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>

#include "core/Window.h"

class Interface {
public:
    void Init(std::shared_ptr<Window> window);
    void Destroy();

    void BeginFrame();
    void DrawUI();
    void Render(VkCommandBuffer*);
};


#endif //ADVANCEDGRAPHICS_INTERFACE_H
