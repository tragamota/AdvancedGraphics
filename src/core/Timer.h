//
// Created by Ian on 23-5-2022.
//

#ifndef MODERN_MODEL_VIEWER_TIMER_H
#define MODERN_MODEL_VIEWER_TIMER_H

#include <chrono>

using std::chrono::steady_clock;

class Timer {
public:
    Timer() {
        Reset();
    }

    void Reset() {
        m_Start = std::chrono::high_resolution_clock::now();
    }

    float Elapsed() {
        using std::chrono::steady_clock;
        using std::chrono::duration;

        steady_clock::time_point stop = steady_clock::now();
        duration<float> runDuration = stop - m_Start;

        return runDuration.count();
    }
private:
    std::chrono::time_point<std::chrono::steady_clock> m_Start;
};


#endif //MODERN_MODEL_VIEWER_TIMER_H
