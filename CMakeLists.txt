cmake_minimum_required(VERSION 3.20)
project(AdvancedGraphics)

set(CMAKE_CXX_STANDARD 20)

# IMGUI PATHS
set(IMGUI_DIR lib/imgui)
set(TINY_GLTF_DIR lib/TinyGltf)

# DAWN OPTIONS
set(DAWN_FETCH_DEPENDENCIES ON)

#GLFW BUILD COMMANDS
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

file(GLOB GLFW_SURFACE_EXTENSION
        src/platform/GlfwSurfaceExtension.h
)

file(GLOB TINY_GLTF
        ${TINY_GLTF_DIR}/json.hpp
        ${TINY_GLTF_DIR}/stbi_image.h
        ${TINY_GLTF_DIR}/stb_image_write.h
        ${TINY_GLTF_DIR}/tiny_gltf.h)

file(GLOB IMGUI_SOURCES
        ${IMGUI_DIR}/backends/imgui_impl_wgpu.h
        ${IMGUI_DIR}/backends/imgui_impl_wgpu.cpp
        ${IMGUI_DIR}/backends/imgui_impl_glfw.h
        ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp

        ${IMGUI_DIR}/*.cpp
        ${IMGUI_DIR}/*.h)

find_package(OpenMP REQUIRED)

# external includes
add_subdirectory(lib/dawn EXCLUDE_FROM_ALL)

include_directories(lib/imgui)
include_directories(lib/TinyGltf)

if (WIN32)
#    include_directories(${PROJECT_NAME} PUBLIC lib/OpenCL/inc)

    list(APPEND GLFW_SURFACE_EXTENSION src/platform/GlfwSurfaceWindows.c)
endif (WIN32)

if(LINUX)
    list(APPEND GLFW_SURFACE_EXTENSION src/platform/GlfwSurfaceUnix.c)
endif(LINUX)

if (APPLE)
    include_directories(${PROJECT_NAME} lib/metal-cpp)

    list(APPEND GLFW_SURFACE_EXTENSION src/platform/GlfwSurfaceMacos.m)
endif (APPLE)

add_executable(AdvancedGraphics ${IMGUI_SOURCES} ${GLFW_SURFACE_EXTENSION} ${TINY_GLTF} src/main.cpp src/core/Window.h src/core/Window.cpp src/core/Application.cpp src/core/Application.h src/utils/constants.h src/utils/constants.h src/rendering/RenderContext.cpp src/rendering/RenderContext.h src/TraceApplication.cpp src/TraceApplication.h src/Interface.cpp src/Interface.h src/rendering/FrameRenderer.cpp src/rendering/FrameRenderer.h
        src/tracer/Accumulator.cpp
        src/tracer/Accumulator.h
        src/tracer/Accumulator.h
        src/tracer/BVH.cpp
        src/tracer/BVH.h
        src/tracer/Ray.cpp
        src/tracer/Ray.h
        src/utils/Math.h
        src/tracer/Scene.cpp
        src/tracer/Scene.h
        src/tracer/SceneLoader.cpp
        src/tracer/SceneLoader.h
        src/tracer/Camera.cpp
        src/tracer/Camera.h
        src/tracer/AABB.cpp
        src/tracer/AABB.h
        src/tracer/PathTracer.cpp
        src/tracer/PathTracer.h
        src/tracer/Texture.cpp
        src/tracer/Texture.h
        src/tracer/Triangle.cpp
        src/tracer/Triangle.h
        src/tracer/BonsaiBVH.cpp
        src/tracer/BonsaiBVH.h
        src/tracer/Mesh.cpp
        src/tracer/Mesh.h
        src/tracer/AreaLight.cpp
        src/tracer/AreaLight.h
        src/tracer/AreaLight.h
        src/tracer/BonsaiMiniTreeSelector.cpp
        src/tracer/BonsaiMiniTreeSelector.h
)

# Link static libraries
target_link_libraries(${PROJECT_NAME} webgpu_dawn webgpu_glfw)
target_link_libraries(${PROJECT_NAME} OpenMP::OpenMP_CXX)

target_compile_options(${PROJECT_NAME} PRIVATE "${OpenMP_CXX_FLAGS}")

if (WIN32)
#    target_link_libraries(${PROJECT_NAME} ${CMAKE_SOURCE_DIR}/lib/OpenCL/lib/OpenCL.lib)
endif (WIN32)

if (APPLE)
    find_library(METAL Metal REQUIRED)
    find_library(FOUNDATION Foundation REQUIRED)
    find_library(QUARTZCORE QuartzCore REQUIRED)

    target_link_libraries(${PROJECT_NAME} ${METAL} ${FOUNDATION} ${QUARTZCORE})
endif (APPLE)

add_custom_command(
        TARGET ${PROJECT_NAME}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets
        ${CMAKE_BINARY_DIR}/assets
)

add_custom_command(
        TARGET ${PROJECT_NAME}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/shaders
        ${CMAKE_BINARY_DIR}/shaders
)

