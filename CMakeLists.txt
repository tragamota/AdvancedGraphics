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
add_subdirectory(lib/highway-1.0.7)

if (WIN32 OR LINUX)
    include_directories(lib/OpenCL/inc)
endif (WIN32 OR LINUX)

if (APPLE)
    add_subdirectory(lib/metal-cpp)
endif (APPLE)

add_executable(AdvancedGraphics src/main.cpp src/Window.h src/Window.cpp)

# Link static libraries
target_link_libraries(${PROJECT_NAME} ${Vulkan_LIBRARIES})
target_link_libraries(${PROJECT_NAME} glfw)

if (WIN32)
    target_link_libraries(${PROJECT_NAME} ${CMAKE_SOURCE_DIR}/lib/OpenCL/lib/OpenCL.lib)
endif (WIN32)

if (APPLE)
    find_library(METAL Metal REQUIRED)
    find_library(FOUNDATION Foundation REQUIRED)
    find_library(QUARTZCORE QuartzCore REQUIRED)

    target_link_libraries(${PROJECT_NAME} ${METAL})
    target_link_libraries(${PROJECT_NAME} ${FOUNDATION})
    target_link_libraries(${PROJECT_NAME} ${QUARTZCORE})
endif (APPLE)

