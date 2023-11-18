#include "TraceApplication.h"

#ifdef WIN32
extern "C"
{
__declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
}

extern "C"
{
__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}
#endif

int main() {
    TraceApplication app;

    app.Init();
    app.Run();
    app.ShutDown();

    return 0;
}

