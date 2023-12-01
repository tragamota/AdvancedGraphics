//
// Created by Ian on 19-11-2023.
//

#include "Interface.h"

void Interface::Init(const std::shared_ptr<Window>& window) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();



//    ImGui_ImplVulkan_InitInfo init_info = {};
//
//    init_info.Instance = *instance;
//    init_info.Device = *device;
//    init_info.PhysicalDevice = *physicalDevice;
//    init_info.QueueFamily = queueFamilyIndex;
//    init_info.Queue = *queue;
//    init_info.ImageCount = 1;
//    init_info.MinImageCount = 1;
//    init_info.UseDynamicRendering = false;
//    init_info.CheckVkResultFn = nullptr;
//    init_info.Subpass = 0;
//    init_info.Allocator = VK_NULL_HANDLE;
//    init_info.DescriptorPool = VK_NULL_HANDLE;
//    init_info.PipelineCache = VK_NULL_HANDLE;
//
//    ImGui_ImplGlfw_InitForVulkan(window->GetGLFWWindow(), false);
//    ImGui_ImplVulkan_Init(init_info, VK_NULL_HANDLE);

    ImGui::StyleColorsDark();
}

void Interface::Destroy() {
//    ImGui_ImplVulkan_Shutdown();
//    ImGui_ImplGlfw_Shutdown();
//    ImGui::DestroyContext();
}

void Interface::BeginFrame() {
//    ImGui_ImplVulkan_NewFrame();
//    ImGui_ImplGlfw_NewFrame();
//    ImGui::NewFrame();
}

void Interface::DrawUI() {
    ImGui::Begin("Demo window");
    ImGui::Button("Hello!");
    ImGui::End();
}

void Interface::Render() {
//    ImGui::Render();
//
//    VkCommandBufferBeginInfo beginInfo = {};
//    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
//    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
//
//    vkBeginCommandBuffer(*commandBuffer, &beginInfo);
}
