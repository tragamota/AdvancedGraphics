//
// Created by Ian on 19-11-2023.
//

#ifndef ADVANCEDGRAPHICS_INTERFACE_H
#define ADVANCEDGRAPHICS_INTERFACE_H

#include <imgui.h>

#include <backends/imgui_impl_wgpu.h>
#include <backends/imgui_impl_glfw.h>

#include "core/Window.h"

class Interface {
public:
    void Init(const std::shared_ptr<Window>& window);
    void Destroy();

    void BeginFrame();
    void DrawUI();
    void Render();
};


#endif //ADVANCEDGRAPHICS_INTERFACE_H
