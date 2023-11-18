//
// Created by Ian on 17-11-2023.
//

#ifndef ADVANCEDGRAPHICS_TRACEAPPLICATION_H
#define ADVANCEDGRAPHICS_TRACEAPPLICATION_H


#include "core/Application.h"
#include "rendering/RenderContext.h"

class TraceApplication : public Application {
public:
    void Init() override;
    void OnTick(double elapsedTime) override;
    void ShutDown() override;
private:
    RenderContext context;
};


#endif //ADVANCEDGRAPHICS_TRACEAPPLICATION_H
