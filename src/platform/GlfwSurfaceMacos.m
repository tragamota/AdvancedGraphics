//
// Created by Ian van de Poll on 07/12/2023.
//

#define GLFW_EXPOSE_NATIVE_COCOA

#include "GlfwSurfaceExtension.h"

#include <Foundation/Foundation.h>
#include <QuartzCore/CAMetalLayer.h>

#include <GLFW/glfw3native.h>

WGPUSurface glfwGetWGPUSurface(WGPUInstance instance, GLFWwindow* window) {
    id metal_layer = NULL;
    NSWindow* ns_window = glfwGetCocoaWindow(window);
    [ns_window.contentView setWantsLayer : YES] ;
    metal_layer = [CAMetalLayer layer];
    [ns_window.contentView setLayer : metal_layer] ;
    return wgpuInstanceCreateSurface(
            instance,
            &(WGPUSurfaceDescriptor){
                    .label = NULL,
                    .nextInChain =
                    (const WGPUChainedStruct*)&(
                            WGPUSurfaceDescriptorFromMetalLayer) {
                                    .chain =
                                    (WGPUChainedStruct){
                                            .next = NULL,
                                            .sType = WGPUSType_SurfaceDescriptorFromMetalLayer,
                                    },
                                    .layer = metal_layer,
                            },
            });
}