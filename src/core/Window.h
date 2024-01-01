//
// Created by Ian van de Poll on 10/11/2023.
//

#ifndef ADVANCEDGRAPHICS_WINDOW_H
#define ADVANCEDGRAPHICS_WINDOW_H

#define GLFW_INCLUDE_NONE

#include <iostream>
#include <any>
#include <functional>

#include "GLFW/glfw3.h"

enum WindowMode {
    WINDOWED,
    WINDOWED_FULLSCREEN,
    FULLSCREEN
};

typedef struct WindowParameters {
    unsigned int width, height;
    unsigned int minWidth = 0, minHeight = 0;
    bool resizable;
    std::unique_ptr<std::string> title;
    WindowMode mode;
} WindowParameters;

typedef struct WindowFrameSize {
    int width;
    int height;
} WindowFrameSize;

class Window {
public:
    explicit Window(const WindowParameters&);
    Window(Window&&) = delete;
    Window(const Window&) = delete;
    ~Window();

    void ChangeTitle(const std::string&) const;
    void ChangeWindowMode(WindowMode) const;
    void ChangeWindowSize(int width, int height) const;
    void ShowWindow() const;
    void HideWindow() const;
    GLFWwindow* GetGLFWWindow();
    [[nodiscard]] WindowFrameSize GetWindowFrameBuffer() const;

    static inline void PollEvents() {
        glfwPollEvents();
    }
    [[nodiscard]] bool ShouldCloseWindow() const;

    std::function<void(int, int)> OnWindowResize;
    std::function<void(int, int)> OnFramebufferResize;
    std::function<void(std::vector<std::string_view>&)> OnPathDrop;
    std::function<void(int, int, int, int)> OnKeyInput;
    std::function<void(double, double)> OnMouseMovement;
    std::function<void(int, int, int)> OnMousePress;
    std::function<void(double, double)> OnMouseScroll;
    std::function<void()> OnClose;
private:
    GLFWwindow* m_Window;
    int m_FrameWidth, m_FrameHeight;

    static void BindWindowCallbacks(GLFWwindow*);
};

#endif //ADVANCEDGRAPHICS_WINDOW_H
