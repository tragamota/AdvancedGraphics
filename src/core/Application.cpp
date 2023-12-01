//
// Created by Ian on 17-11-2023.
//

#include "Application.h"

void Application::Init() {
    if (!glfwInit()) {
        std::cout << "ERROR INITIALIZING OF GLFW FAILED" << std::endl;
        return;
    }

    m_MainWindow = std::make_shared<Window>(WindowParameters{
        1280, 720,
        720, 540,
        true,
        std::make_unique<std::string>("Advanced Graphics"),
        WINDOWED});

    m_MainWindow->ShowWindow();
}

void Application::ShutDown() {
    glfwTerminate();
}

void Application::Run() {
    double elapsedTime = 0;

//    m_MainWindow->ShowWindow();

    while (!m_MainWindow->ShouldCloseWindow()) {
        elapsedTime = m_Timer.Elapsed();
        m_Timer.Reset();

        OnTick(elapsedTime);
        Window::PollEvents();
    }

    m_MainWindow->HideWindow();
}
