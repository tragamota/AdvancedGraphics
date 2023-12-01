//
// Created by Ian on 17-11-2023.
//

#include "TraceApplication.h"

void TraceApplication::Init() {
    Application::Init();

    m_Context.Init(m_MainWindow);
    m_Context.RegisterInterface(m_MainWindow, &m_Interface);
}

void TraceApplication::OnTick(double elapsedTime) {
    auto renderPass = m_Context.GetRenderPass();

    m_Interface.BeginFrame();

    m_Interface.DrawUI();
    m_Interface.EndFrame();
    m_Interface.Render(&renderPass);

    wgpuRenderPassEncoderEnd(renderPass);

    m_Context.SubmitCommandBuffer();
    m_Context.Present();

    // std::cout << "Tick happened with an elapsed time of : " << elapsedTime << std::endl;
}

void TraceApplication::ShutDown() {
    m_Interface.Destroy();
    m_Context.Destroy();

    Application::ShutDown();
}


