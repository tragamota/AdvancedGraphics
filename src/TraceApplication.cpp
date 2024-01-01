//
// Created by Ian on 17-11-2023.
//

#include "TraceApplication.h"


void TraceApplication::Init() {
    Application::Init();

    m_MainWindow->OnFramebufferResize = [this](int width, int height) {
        m_ResizeRequested = true;
    };

    auto contextResources = m_Context.GetContextResources();

    m_Context.Init(m_MainWindow);
    m_Interface.Init(m_MainWindow, contextResources);
    m_FrameRenderer.Init(contextResources, m_MainWindow->GetWindowFrameBuffer());
}

void TraceApplication::OnTick(double elapsedTime) {
    Update();
    Render();

    // std::cout << "Tick happened with an elapsed time of : " << elapsedTime << std::endl;
}

void TraceApplication::Update() {
    if(m_ResizeRequested) {
        m_Context.ResizeSwapChain(m_MainWindow->GetWindowFrameBuffer());
        m_ResizeRequested = false;
    }

    wgpuDeviceTick(*m_Context.GetContextResources().device);
}

void TraceApplication::Render() {
    auto renderPass = m_Context.GetRenderPass();

    m_Interface.BeginFrame();
    m_Interface.DrawUI();
    m_Interface.EndFrame();

    m_FrameRenderer.Draw(renderPass);

    m_Interface.Render(renderPass);

    m_Context.SubmitCommandBuffer(1);
    m_Context.Present();
}

void TraceApplication::ShutDown() {
    m_Interface.Destroy();
    m_Context.Destroy();

    Application::ShutDown();
}




