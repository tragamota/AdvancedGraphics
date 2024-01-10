//
// Created by Ian on 2-1-2024.
//

#include "Accumulator.h"

#include <omp.h>

Accumulator::Accumulator(WindowFrameSize frameSize) : m_Film(frameSize) {
    ConstructAccumulator();
    Clear();
}

Accumulator::~Accumulator() {
    RemoveAccumulator();
}

uint32_t* Accumulator::GetImage(uint32_t totalFrames)  {
    #pragma omp parallel for schedule(dynamic)
    for(int y = 0; y < m_Film.height; y++) {
        #pragma omp parallel for schedule(dynamic)
        for(int x = 0; x < m_Film.width; x++) {
            auto texel = &m_TextureBuffer[y * m_Film.width + x];
            auto& accumulatorTexel = m_Accumulator[y * m_Film.width + x];

            auto accumulatorTexelNormalized = accumulatorTexel / (float) (totalFrames + 1);

            auto r = (uint8_t) (accumulatorTexelNormalized.x * 255.0f);
            auto g = (uint8_t) (accumulatorTexelNormalized.y * 255.0f);
            auto b = (uint8_t) (accumulatorTexelNormalized.z * 255.0f);
            auto a = (uint8_t) (accumulatorTexelNormalized.w * 255.0f);

            *texel = (a << 24) | (b << 16) | (g << 8) | r;
        }
    }

    return m_TextureBuffer;
}

void Accumulator::AccumulateValue(vec4f& value, uint32_t x, uint32_t y) {
    m_Accumulator[y * m_Film.width + x] = value;
}

void Accumulator::Resize(WindowFrameSize& frameSize) {
    m_Film = frameSize;

    RemoveAccumulator();
    ConstructAccumulator();
}

inline void Accumulator::ConstructAccumulator() {
    m_Accumulator = new vec4f[m_Film.width * m_Film.height];
    m_TextureBuffer = new uint32_t[m_Film.width * m_Film.height];
}

inline void Accumulator::RemoveAccumulator() {
    delete[] m_Accumulator;
    delete[] m_TextureBuffer;
}

void Accumulator::Clear() {
    memset(m_TextureBuffer, 0, sizeof(uint32_t) * m_Film.width * m_Film.height);
    memset(m_Accumulator, 0, m_Film.width * m_Film.height * sizeof(vec4f));
}
