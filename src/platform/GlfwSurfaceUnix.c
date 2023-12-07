//
// Created by Ian van de Poll on 07/12/2023.
//

#include "GlfwSurfaceExtension.h"

#ifdef _GLFW_WAYLAND
    #define GLFW_EXPOSE_NATIVE_WAYLAND
#else
    #define GLFW_EXPOSE_NATIVE_X11
#endif

#include <GLFW/glfw3native.h>

#ifdef _GLFW_WAYLAND

WGPUSurface glfwGetWGPUSurface(WGPUInstance instance, GLFWwindow* window) {
    struct wl_display* wayland_display = glfwGetWaylandDisplay();
    struct wl_surface* wayland_surface = glfwGetWaylandWindow(window);
    return wgpuInstanceCreateSurface(
            instance,
            &(WGPUSurfaceDescriptor){
                    .label = NULL,
                    .nextInChain =
                    (const WGPUChainedStruct*)&(
                            WGPUSurfaceDescriptorFromWaylandSurface) {
                                    .chain =
                                    (WGPUChainedStruct){
                                            .next = NULL,
                                            .sType =
                                            WGPUSType_SurfaceDescriptorFromWaylandSurface,
                                    },
                                    .display = wayland_display,
                                    .surface = wayland_surface,
                            },
            });
}

#else

WGPUSurface glfwGetWGPUSurface(WGPUInstance instance, GLFWwindow* window) {
    Display* x11_display = glfwGetX11Display();
    Window x11_window = glfwGetX11Window(window);
    return wgpuInstanceCreateSurface(
            instance,
            &(WGPUSurfaceDescriptor){
                    .label = NULL,
                    .nextInChain =
                    (const WGPUChainedStruct*)&(
                            WGPUSurfaceDescriptorFromXlibWindow) {
                                    .chain =
                                    (WGPUChainedStruct){
                                            .next = NULL,
                                            .sType = WGPUSType_SurfaceDescriptorFromXlibWindow,
                                    },
                                    .display = x11_display,
                                    .window = x11_window,
                            },
            });
}

#endif