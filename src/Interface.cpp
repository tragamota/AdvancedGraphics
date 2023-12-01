//
// Created by Ian on 19-11-2023.
//

#include "Interface.h"

void Interface::Init(const std::shared_ptr<Window>& window, WGPUDevice* device, WGPUTextureFormat swapChainFormat) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();

    auto a = ImGui_ImplGlfw_InitForOther(window->GetGLFWWindow(), true);
    auto b = ImGui_ImplWGPU_Init(*device, 2,swapChainFormat);

    ImGui::StyleColorsDark();
}

void Interface::Destroy() {
    ImGui_ImplWGPU_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void Interface::BeginFrame() {
    ImGui_ImplWGPU_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void Interface::EndFrame() {
    ImGui::EndFrame();
}

void Interface::DrawUI() {
    ImGui::Begin("Demo window");
    ImGui::Button("Hello!");
    ImGui::End();
}

void Interface::Render(WGPURenderPassEncoder* renderPassEncoder) {
    ImGui::Render();

    ImGui_ImplWGPU_RenderDrawData(ImGui::GetDrawData(), *renderPassEncoder);
}
