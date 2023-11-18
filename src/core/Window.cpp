//
// Created by Ian on 15-11-2023.
//

#include "Window.h"

inline Window* CastWindowPointer(GLFWwindow* window) {
    return static_cast<Window*>(glfwGetWindowUserPointer(window));
}

inline void bindWindowCallbacks(GLFWwindow* window) {
    glfwSetDropCallback(window, [](GLFWwindow* caller, int drops, const char** c_paths)  {
        auto windowStructure = CastWindowPointer(caller);

        if(windowStructure->OnPathDrop) {
            std::vector<std::string_view> paths {};
            paths.reserve(drops);

            for(int i = 0; i < drops; i++)
                paths.emplace_back(c_paths[i]);

            windowStructure->OnPathDrop(paths);
        }
    });

    glfwSetWindowSizeCallback(window, [](GLFWwindow* caller, int width, int height) {
        auto windowStructure = CastWindowPointer(caller);

        printf("Window resize");

        if(windowStructure->OnWindowResize) {
            windowStructure->OnWindowResize(width, height);
        }
    });

    glfwSetFramebufferSizeCallback(window, [](GLFWwindow* caller, int width, int height) {
        auto windowStructure = CastWindowPointer(caller);

        if(windowStructure->OnFramebufferResize) {
            windowStructure->OnFramebufferResize(width, height);
        }
    });

    glfwSetMouseButtonCallback(window, [](GLFWwindow* caller, int button, int action, int mods) {
        auto windowStructure = CastWindowPointer(caller);

        if(windowStructure->OnMousePress) {
            windowStructure->OnMousePress(button, action, mods);
        }
    });

    glfwSetCursorPosCallback(window, [](GLFWwindow* caller, double width, double height) {
        auto windowStructure = CastWindowPointer(caller);

        if(windowStructure->OnMouseMovement) {
            windowStructure->OnMouseMovement(width, height);
        }
    });

    glfwSetScrollCallback(window, [](GLFWwindow* caller, double horizontalScroll, double verticalScroll) {
        auto windowStructure = CastWindowPointer(caller);

        if(windowStructure->OnMouseScroll) {
            windowStructure->OnMouseScroll(horizontalScroll, verticalScroll);
        }
    });

    glfwSetWindowCloseCallback(window, [](GLFWwindow* caller) {
        auto windowStructure = CastWindowPointer(caller);

        if(windowStructure->OnClose) {
            windowStructure->OnClose();
        }
    });

    glfwSetKeyCallback(window, [](GLFWwindow* caller, int key, int scancode, int action, int mods) {
        auto windowStructure = CastWindowPointer(caller);

        if(windowStructure->OnKeyInput) {
            windowStructure->OnKeyInput(key, scancode, action, mods);
        }
    });
}


Window::Window(const WindowParameters& params) {
    glfwWindowHint(GLFW_VISIBLE, false);

    m_Window = glfwCreateWindow(params.width, params.height, params.title->c_str(), nullptr, nullptr);

    if(m_Window == nullptr) {
        return;
    }

    if (params.minWidth != 0 && params.minHeight != 0) {
        glfwSetWindowSizeLimits(m_Window, params.minWidth, params.minHeight, GLFW_DONT_CARE, GLFW_DONT_CARE);
    }

    glfwSetWindowOpacity(m_Window, 0.95);
    glfwSetWindowAttrib(m_Window, GLFW_RESIZABLE, params.resizable);
    glfwSetWindowUserPointer(m_Window, this);

    bindWindowCallbacks(m_Window);
}

Window::~Window()  {
    glfwDestroyWindow(m_Window);
}

void Window::ChangeWindowMode(const WindowMode windowMode) const {
    auto primaryMonitor = glfwGetPrimaryMonitor();
    auto videoMode = glfwGetVideoMode(primaryMonitor);

    switch (windowMode) {
        case FULLSCREEN:
            glfwSetWindowMonitor(m_Window, primaryMonitor, 0, 0, videoMode->width, videoMode->height, videoMode->refreshRate);
            break;
        case WINDOWED_FULLSCREEN:
            glfwSetWindowAttrib(m_Window, GLFW_RED_BITS, videoMode->redBits);
            glfwSetWindowAttrib(m_Window, GLFW_GREEN_BITS, videoMode->greenBits);
            glfwSetWindowAttrib(m_Window, GLFW_BLUE_BITS, videoMode->blueBits);
            glfwSetWindowAttrib(m_Window, GLFW_REFRESH_RATE, videoMode->refreshRate);

            glfwSetWindowMonitor(m_Window, primaryMonitor, 0, 0, videoMode->width, videoMode->height, videoMode->refreshRate);
            break;
        case WINDOWED:
            glfwSetWindowMonitor(m_Window, nullptr, 0, 0, 0, 0, 0);
            break;
    }
}

void Window::ShowWindow() const {
    glfwShowWindow(m_Window);
}

void Window::HideWindow() const {
    glfwHideWindow(m_Window);
}

bool Window::ShouldCloseWindow() const {
    return glfwWindowShouldClose(m_Window);
}

void Window::ChangeTitle(const std::string& newTitle) const {
    glfwSetWindowTitle(m_Window, newTitle.c_str());
}

void Window::ChangeWindowSize(int width, int height) const {
    glfwSetWindowSize(m_Window, width, height);
}

GLFWwindow* Window::GetGLFWWindow() {
    return m_Window;
}
