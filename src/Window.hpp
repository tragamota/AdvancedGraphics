//
// Created by Ian van de Poll on 10/11/2023.
//

#ifndef ADVANCEDGRAPHICS_WINDOW_HPP
#define ADVANCEDGRAPHICS_WINDOW_HPP

#include <iostream>
#include <any>

enum WindowMode {
    WINDOWED,
    WINDOWED_FULLSCREEN,
    FULLSCREEN
};

typedef struct WindowParameters {
    int width, height;
    int minWidth, minHeight;
    bool resizable;
    std::string_view title;
    WindowMode mode;
};

class Window {
public:
    Window();
    Window(Window&&) = delete;
    Window(const Window&) = delete;
    ~Window();

    std::any GetNativeWindowHandle();
    void ChangeTitle(const std::string&);
    void ChangeWindowMode(const WindowMode);
    void ShowWindow();
    void HideWindow();

    void PollEvents();
    bool ShouldCloseWindow();

    std::function<void(int, int)> OnWindowResize;
    std::function<void(int, int)> OnFramebufferResize;
    std::function<void(std::vector<std::string_view>&)> OnPathDrop;
    std::function<void(int, int, int, int)> OnKeyInput;
    std::function<void(double, double)> OnMouseMovement;
    std::function<void(int, int, int)> OnMousePress;
    std::function<void(int, double, double)> OnMouseScroll;
    std::function<void()> OnClose;
};


#endif //ADVANCEDGRAPHICS_WINDOW_HPP
