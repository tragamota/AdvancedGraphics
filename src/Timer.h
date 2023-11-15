//
// Created by Ian on 23-5-2022.
//

#ifndef MODERN_MODEL_VIEWER_TIMER_H
#define MODERN_MODEL_VIEWER_TIMER_H

#include <chrono>

class Timer {
public:
    Timer() {
        Reset();
    }

    void Reset()
    {
        m_Start = std::chrono::high_resolution_clock::now();
    }

    double Elapsed()
    {
        return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - m_Start).count() * 0.001f * 0.001f * 0.001f;
    }

    float ElapsedMillis()
    {
        return Elapsed() * 1000.0f;
    }

private:
    std::chrono::time_point<std::chrono::high_resolution_clock> m_Start;
};


#endif //MODERN_MODEL_VIEWER_TIMER_H
