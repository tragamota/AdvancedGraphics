//
// Created by Ian on 17-11-2023.
//

#ifndef ADVANCEDGRAPHICS_APPLICATION_H
#define ADVANCEDGRAPHICS_APPLICATION_H

#include "Window.h"
#include "Timer.h"

class Application {
public:
    Application() = default;
    Application(const Window &otherWindow) = delete;
    virtual ~Application() = default;

    virtual void Init();
    virtual void ShutDown();

    void Run();
protected:
    std::shared_ptr<Window> m_MainWindow;

    virtual void OnTick(float elapsedTime) = 0;
private:
    Timer m_Timer;
};


#endif //ADVANCEDGRAPHICS_APPLICATION_H
