cmake_minimum_required(VERSION 3.20)
project(AdvancedGraphics)

set(CMAKE_CXX_STANDARD 17)

#GLFW build commands
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

find_package(Vulkan REQUIRED)

# external includes
add_subdirectory(lib/GLFW lib/imgui)
include_directories(lib/hwy/include)
include_directories(${Vulkan_INCLUDE_DIRS})

if (WIN32 OR LINUX)
    include_directories(${PROJECT_NAME} PUBLIC lib/OpenCL/inc)
endif (WIN32 OR LINUX)

if (APPLE)
    target_include_directories(${PROJECT_NAME} PRIVATE lib/metal-cpp)
endif (APPLE)

add_executable(AdvancedGraphics src/main.cpp src/core/Window.h src/core/Window.cpp src/core/Application.cpp src/core/Application.h src/utils/constants.h src/utils/constants.h src/rendering/RenderContext.cpp src/rendering/RenderContext.h src/TraceApplication.cpp src/TraceApplication.h src/rendering/RenderDevice.cpp src/rendering/RenderDevice.h src/rendering/RenderDeviceManager.cpp src/rendering/RenderDeviceManager.h src/rendering/RenderDeviceManager.h)

# Link static libraries
target_link_libraries(${PROJECT_NAME} ${Vulkan_LIBRARIES})
target_link_libraries(${PROJECT_NAME} glfw)

if (WIN32)
    target_link_libraries(${PROJECT_NAME} ${CMAKE_SOURCE_DIR}/lib/OpenCL/lib/OpenCL.lib)
    target_link_libraries(${PROJECT_NAME} ${CMAKE_SOURCE_DIR}/lib/hwy/lib/win32/hwy.lib)
    target_link_libraries(${PROJECT_NAME} ${CMAKE_SOURCE_DIR}/lib/hwy/lib/win32/hwy_contrib.lib)
endif (WIN32)

if (APPLE)
    find_library(METAL Metal REQUIRED)
    find_library(FOUNDATION Foundation REQUIRED)
    find_library(QUARTZCORE QuartzCore REQUIRED)

    target_link_libraries(${PROJECT_NAME} ${METAL})
    target_link_libraries(${PROJECT_NAME} ${FOUNDATION})
    target_link_libraries(${PROJECT_NAME} ${QUARTZCORE})
endif (APPLE)

