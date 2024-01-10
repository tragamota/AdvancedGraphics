//
// Created by Ian on 2-1-2024.
//

#ifndef ADVANCEDGRAPHICS_ACCUMULATOR_H
#define ADVANCEDGRAPHICS_ACCUMULATOR_H

#include "../core/Window.h"
#include "../utils/Math.h"

class Accumulator {
    vec4f* m_Accumulator;
    uint32_t* m_TextureBuffer;
    WindowFrameSize m_Film;

    void ConstructAccumulator();
    void RemoveAccumulator();
public:
    Accumulator() = default;
    Accumulator(WindowFrameSize);
    ~Accumulator();

    uint32_t* GetImage(uint32_t totalFrames);
    void AccumulateValue(vec4f&, uint32_t x, uint32_t y);
    void Resize(WindowFrameSize&);
    void Clear();
};


#endif //ADVANCEDGRAPHICS_ACCUMULATOR_H
