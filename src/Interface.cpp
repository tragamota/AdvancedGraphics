//
// Created by Ian on 19-11-2023.
//

#include "Interface.h"

void Interface::Init(std::shared_ptr<Window> window) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();

    ImGui_ImplVulkan_InitInfo init_info = {

    };

    ImGui_ImplGlfw_InitForVulkan(window->GetGLFWWindow(), false);
    ImGui_ImplVulkan_Init(init_info, nullptr);

    ImGui::StyleColorsDark();
}

void Interface::Destroy() {
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void Interface::BeginFrame() {
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void Interface::DrawUI() {
    ImGui::Begin("Demo window");
    ImGui::Button("Hello!");
    ImGui::End();
}

void Interface::Render(VkCommandBuffer* commandBuffer) {
    ImGui::Render();

    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(*commandBuffer, &beginInfo);
}
