//
// Created by Ian van de Poll on 07/12/2023.
//

#include "GlfwSurfaceExtension.h"

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

WGPUSurface glfwGetWGPUSurface(WGPUInstance instance, GLFWwindow* window) {
    HWND hwnd = glfwGetWin32Window(window);
    HINSTANCE hinstance = GetModuleHandle(NULL);

    return wgpuInstanceCreateSurface(
        instance,
        &(WGPUSurfaceDescriptor){
        .label = NULL,
        .nextInChain =
        (const WGPUChainedStruct*)&(
            WGPUSurfaceDescriptorFromWindowsHWND) {
                    .chain =
                    (WGPUChainedStruct){
                            .next = NULL,
                            .sType = WGPUSType_SurfaceDescriptorFromWindowsHWND,
                    },
                    .hinstance = hinstance,
                    .hwnd = hwnd,
            },
    });
}