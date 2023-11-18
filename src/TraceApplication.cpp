//
// Created by Ian on 17-11-2023.
//

#include "TraceApplication.h"
#include "rendering/RenderContext.h"

void TraceApplication::Init() {
    Application::Init();

    context.Init();
}

void TraceApplication::OnTick(double elapsedTime) {
//    std::cout << "Tick happened with an elapsed time of : " << elapsedTime << std::endl;
}

void TraceApplication::ShutDown() {
    Application::ShutDown();

    context.Destroy();
}


