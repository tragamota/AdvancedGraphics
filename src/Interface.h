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

    void Init(std::shared_ptr<Window> window, ImGui_ImplVulkan_InitInfo *vulkanInitInfo) {

        VkDescriptorPoolSize pool_sizes[] =
                {
                        {VK_DESCRIPTOR_TYPE_SAMPLER,                1000},
                        {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000},
                        {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,          1000},
                        {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,          1000},
                        {VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER,   1000},
                        {VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER,   1000},
                        {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,         1000},
                        {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,         1000},
                        {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000},
                        {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000},
                        {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,       1000}
                };

        VkDescriptorPoolCreateInfo pool_info = {};
        pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
        pool_info.maxSets = 1000;
        pool_info.poolSizeCount = std::size(pool_sizes);
        pool_info.pPoolSizes = pool_sizes;

        VkDescriptorPool imguiPool;
        vkCreateDescriptorPool(_device, &pool_info, nullptr, &imguiPool);

        ImGui::CreateContext();
        ImGui_ImplGlfw_InitForVulkan(window->GetGLFWWindow(), false);

        vulkanInitInfo.DescriptorPool = imguiPool;
        vulkanInitInfo.MinImageCount = 3;
        vulkanInitInfo.ImageCount = 3;
        vulkanInitInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

        ImGui_ImplVulkan_Init(vulkanInitInfo, nullptr);

        immediate_submit([&](VkCommandBuffer cmd) {
            ImGui_ImplVulkan_CreateFontsTexture(cmd);
        });

        //clear font textures from cpu data
        //ImGui_ImplVulkan_DestroyFontUploadObjects();
    }


};


#endif //ADVANCEDGRAPHICS_INTERFACE_H
