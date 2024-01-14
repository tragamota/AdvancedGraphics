//
// Created by Ian on 17-11-2023.
//

#include "TraceApplication.h"


void TraceApplication::Init() {
    Application::Init();

    m_MainWindow->OnFramebufferResize = [this](int width, int height) {
        m_ResizeRequested = true;
    };

    m_MainWindow->OnMouseMovement = [this](double x, double y) {

    };

    m_MainWindow->OnKeyInput = [this](int key, int scanCode, int action, int mods) {
        if(action == GLFW_REPEAT)
            action = GLFW_PRESS;

        keyState[key] = action;
    };

    auto contextResources = m_Context.GetContextResources();
    auto windowFrameSize = m_MainWindow->GetWindowFrameBuffer();

    m_Context.Init(m_MainWindow);
    m_Interface.Init(m_MainWindow, contextResources);
    m_FrameRenderer.Init(contextResources, windowFrameSize);

    m_PathTracer = new PathTracer("assets/sky_19.hdr", windowFrameSize);

    m_Camera = m_PathTracer->GetCamera();
    m_Accumulator = m_PathTracer->GetAccumulator();
}

void TraceApplication::OnTick(float elapsedTime) {
    Update(elapsedTime);
    Render();
}

void TraceApplication::Update(float elapsedTime) {
    if(m_ResizeRequested) {
        auto windowFrame = m_MainWindow->GetWindowFrameBuffer();

        m_Context.ResizeSwapChain(windowFrame);
        m_FrameRenderer.ResizeAccumulatorTexture(windowFrame);
        m_PathTracer->Resize(windowFrame);
        m_ResizeRequested = false;
    }

    if(keyState[GLFW_KEY_F1] == GLFW_PRESS && !controlStates.hasToggledEnableMouse) {
        controlStates.enableMouse = !controlStates.enableMouse;
        controlStates.hasToggledEnableMouse = true;
    }

    if(keyState[GLFW_KEY_F1] == GLFW_RELEASE && controlStates.hasToggledEnableMouse) {
        controlStates.hasToggledEnableMouse = false;
    }

    if(keyState[GLFW_KEY_W] == GLFW_PRESS ) {
        m_Camera->MoveForward(elapsedTime);
    }

    if(keyState[GLFW_KEY_A] == GLFW_PRESS) {
        m_Camera->MoveLeft(elapsedTime);
    }

    if(keyState[GLFW_KEY_S] == GLFW_PRESS) {
        m_Camera->MoveBackward(elapsedTime);
    }

    if(keyState[GLFW_KEY_D] == GLFW_PRESS) {
        m_Camera->MoveRight(elapsedTime);
    }

    if(keyState[GLFW_KEY_LEFT_SHIFT] == GLFW_PRESS) {
        m_Camera->MoveUp(elapsedTime);
    }

    if(keyState[GLFW_KEY_LEFT_CONTROL] == GLFW_PRESS) {
        m_Camera->MoveDown(elapsedTime);
    }

    if(controlStates.enableMouse) {
        m_Camera->ChangeOrientation(1, 1);
        std::cout << m_Camera->m_Target.x << ", " << m_Camera->m_Target.y << ", " << m_Camera->m_Target.z  << std::endl;
    }

    m_FrameRenderer.CopyAccumulatorToTexture(m_Accumulator->GetImage(0));

    wgpuDeviceTick(*m_Context.GetContextResources().device);
}

void TraceApplication::Render() {
    auto renderPass = m_Context.GetRenderPass();

    m_PathTracer->RenderFrame();

    m_Interface.BeginFrame();
    m_Interface.DrawUI();
    m_Interface.EndFrame();

    m_FrameRenderer.Draw(renderPass);

    m_Interface.Render(renderPass);

    m_Context.SubmitCommandBuffer();
    m_Context.Present();
}

void TraceApplication::ShutDown() {
    m_Interface.Destroy();
    m_Context.Destroy();

    delete m_PathTracer;

    Application::ShutDown();
}




