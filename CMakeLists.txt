cmake_minimum_required(VERSION 3.20)
project(AdvancedGraphics)

set(CMAKE_CXX_STANDARD 20)

# IMGUI PATHS
set(IMGUI_DIR lib/imgui)

# DAWN OPTIONS
set(DAWN_FETCH_DEPENDENCIES ON)

#GLFW BUILD COMMANDS
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

file(GLOB IMGUI_SOURCES
        ${IMGUI_DIR}/backends/imgui_impl_wgpu.h
        ${IMGUI_DIR}/backends/imgui_impl_wgpu.cpp
        ${IMGUI_DIR}/backends/imgui_impl_glfw.h
        ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp

        ${IMGUI_DIR}/*.cpp
        ${IMGUI_DIR}/*.h)

# external includes
add_subdirectory(lib/dawn EXCLUDE_FROM_ALL)

include_directories(lib/imgui)
#include_directories(lib/hwy/include)

if (WIN32 OR LINUX)
    include_directories(${PROJECT_NAME} PUBLIC lib/OpenCL/inc)
endif (WIN32 OR LINUX)

if (APPLE)
    include_directories(${PROJECT_NAME} lib/metal-cpp)
endif (APPLE)

add_executable(AdvancedGraphics ${IMGUI_SOURCES} src/main.cpp src/core/Window.h src/core/Window.cpp src/core/Application.cpp src/core/Application.h src/utils/constants.h src/utils/constants.h src/rendering/RenderContext.cpp src/rendering/RenderContext.h src/TraceApplication.cpp src/TraceApplication.h src/Interface.cpp src/Interface.h src/misc/glfw3webgpu.c)

# Link static libraries
target_link_libraries(${PROJECT_NAME} webgpu_cpp webgpu_dawn webgpu_glfw)

if (WIN32)
    target_link_libraries(${PROJECT_NAME} ${CMAKE_SOURCE_DIR}/lib/OpenCL/lib/OpenCL.lib)
#    target_link_libraries(${PROJECT_NAME} ${CMAKE_SOURCE_DIR}/lib/hwy/lib/win32/hwy.lib)
#    target_link_libraries(${PROJECT_NAME} ${CMAKE_SOURCE_DIR}/lib/hwy/lib/win32/hwy_contrib.lib)
endif (WIN32)

if (APPLE)
    find_library(METAL Metal REQUIRED)
    find_library(FOUNDATION Foundation REQUIRED)
    find_library(QUARTZCORE QuartzCore REQUIRED)

    target_link_libraries(${PROJECT_NAME} ${METAL})
    target_link_libraries(${PROJECT_NAME} ${FOUNDATION})
    target_link_libraries(${PROJECT_NAME} ${QUARTZCORE})
endif (APPLE)

