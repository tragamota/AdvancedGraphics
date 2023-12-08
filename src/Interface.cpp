//
// Created by Ian on 19-11-2023.
//

#include "Interface.h"

void Interface::Init(const std::shared_ptr<Window>& window, WGPUDevice* device, WGPUTextureFormat swapChainFormat) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();

    ImGui_ImplGlfw_InitForOther(window->GetGLFWWindow(), true);
    ImGui_ImplWGPU_Init(*device, 2,swapChainFormat);

    AdaptWindowScaling(window->GetGLFWWindow());

    io.Fonts->AddFontFromFileTTF("assets/Roboto.ttf", io.DisplayFramebufferScale.x * 13.0f, nullptr, nullptr);
    io.FontAllowUserScaling = true;
    io.FontGlobalScale = 1.0f / io.DisplayFramebufferScale.x;
    ImGui::StyleColorsDark();
}

void Interface::Destroy() {
    ImGui_ImplWGPU_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void Interface::AdaptWindowScaling(GLFWwindow* window) {
    ImGuiIO& io = ImGui::GetIO();
    float xScale, yScale;

    glfwGetWindowContentScale(window, &xScale, &yScale);

    io.DisplayFramebufferScale = ImVec2(xScale, yScale);
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
