//
// Created by Ian on 17-11-2023.
//

#include "TraceApplication.h"


void TraceApplication::Init() {
    Application::Init();

    m_MainWindow->OnFramebufferResize = [this](int width, int height) {
        controlStates.m_ResizeRequested = true;
    };

    m_MainWindow->OnMouseMovement = [this](double x, double y) {
        controlStates.currentMousePosition = vec2f((float) x,(float) y);
    };

    m_MainWindow->OnMousePress = [this](int button, int state, int mods) {
        if(button == GLFW_MOUSE_BUTTON_RIGHT && state == GLFW_PRESS)
            controlStates.leftMouseButtonPressed = true;
        else {
            controlStates.leftMouseButtonPressed = false;
        }
    };

    m_MainWindow->OnMouseScroll = [this](double x, double y) {
        m_Camera->ChangeFov(m_Camera->GetFieldOfView() + y * 1);
    };

    m_MainWindow->OnKeyInput = [this](int key, int scanCode, int action, int mods) {
        if(action == GLFW_REPEAT)
            action = GLFW_PRESS;

        controlStates.keyState[key] = action;
    };

    auto contextResources = m_Context.GetContextResources();
    auto windowFrameSize = m_MainWindow->GetWindowFrameBuffer();

    m_Context.Init(m_MainWindow);
    m_Interface.Init(m_MainWindow, contextResources);
    m_FrameRenderer.Init(contextResources, windowFrameSize);

    m_PathTracer = new PathTracer("assets/skydome_night.hdr", windowFrameSize);

    m_Camera = m_PathTracer->GetCamera();
    m_Accumulator = m_PathTracer->GetAccumulator();
}

void TraceApplication::OnTick(float elapsedTime) {
    Update(elapsedTime);
    Render();
}

void TraceApplication::Update(float elapsedTime) {
    if(controlStates.m_ResizeRequested) {
        auto windowFrame = m_MainWindow->GetWindowFrameBuffer();

        m_Context.ResizeSwapChain(windowFrame);
        m_FrameRenderer.ResizeAccumulatorTexture(windowFrame);
        m_PathTracer->Resize(windowFrame);
        controlStates.m_ResizeRequested = false;
    }

    if(controlStates.keyState[GLFW_KEY_F1] == GLFW_PRESS && !controlStates.hasToggledEnableMouse) {
        controlStates.enableMouse = !controlStates.enableMouse;
        controlStates.hasToggledEnableMouse = true;
    }

    if(controlStates.keyState[GLFW_KEY_F1] == GLFW_RELEASE && controlStates.hasToggledEnableMouse) {
        controlStates.hasToggledEnableMouse = false;
    }

    if(controlStates.keyState[GLFW_KEY_W] == GLFW_PRESS ) {
        m_Camera->MoveForward(elapsedTime);
    }

    if(controlStates.keyState[GLFW_KEY_A] == GLFW_PRESS) {
        m_Camera->MoveLeft(elapsedTime);
    }

    if(controlStates.keyState[GLFW_KEY_S] == GLFW_PRESS) {
        m_Camera->MoveBackward(elapsedTime);
    }

    if(controlStates.keyState[GLFW_KEY_D] == GLFW_PRESS) {
        m_Camera->MoveRight(elapsedTime);
    }

    if(controlStates.keyState[GLFW_KEY_LEFT_SHIFT] == GLFW_PRESS) {
        m_Camera->MoveUp(elapsedTime);
    }

    if(controlStates.keyState[GLFW_KEY_LEFT_CONTROL] == GLFW_PRESS) {
        m_Camera->MoveDown(elapsedTime);
    }

    if(controlStates.enableMouse) {
        vec2f relativeMouseMovement = controlStates.currentMousePosition - controlStates.lastMousePosition;
        if(controlStates.leftMouseButtonPressed) {
            if (relativeMouseMovement.x != 0 || relativeMouseMovement.y != 0) {
                m_Camera->ChangeOrientation(relativeMouseMovement.x, relativeMouseMovement.y, elapsedTime);
            }
        }
    }

    controlStates.lastMousePosition = controlStates.currentMousePosition;

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




