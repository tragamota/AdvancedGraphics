//
// Created by Ian on 19-11-2023.
//

#ifndef ADVANCEDGRAPHICS_INTERFACE_H
#define ADVANCEDGRAPHICS_INTERFACE_H

#include <dawn/webgpu.h>

#include <imgui.h>
#include <backends/imgui_impl_wgpu.h>
#include <backends/imgui_impl_glfw.h>

#include "rendering/RenderContext.h"
#include "core/Window.h"

class Interface {
    void AdaptWindowScaling(GLFWwindow *window);

public:
    void Init(const std::shared_ptr<Window>& window, ContextResources&);
    void Destroy();

    void BeginFrame();
    void EndFrame();

    void DrawUI();
    int Render(const WGPURenderPassEncoder *);
};


#endif //ADVANCEDGRAPHICS_INTERFACE_H
