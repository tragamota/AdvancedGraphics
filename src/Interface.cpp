//
// Created by Ian on 19-11-2023.
//

#include "Interface.h"

void Interface::Init(const std::shared_ptr<Window>& window, ContextResources& contextResources) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();

    ImGui_ImplGlfw_InitForOther(window->GetGLFWWindow(), true);
    ImGui_ImplWGPU_Init(*contextResources.device, 2, contextResources.surfaceFormat);

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
    ImGui::Render();
}

void Interface::DrawUI() {
    auto io = ImGui::GetIO();

    ImGui::Begin("Demo window");
    ImGui::Button("Hello!");
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
    ImGui::End();
}

int Interface::Render(const WGPURenderPassEncoder* renderPassEncoder) {
    ImDrawData* drawData = ImGui::GetDrawData();
    ImGui_ImplWGPU_RenderDrawData(drawData, *renderPassEncoder);

    return drawData->CmdListsCount;
}
