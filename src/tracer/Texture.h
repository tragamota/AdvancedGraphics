//
// Created by Ian on 8-1-2024.
//

#ifndef ADVANCEDGRAPHICS_TEXTURE_H
#define ADVANCEDGRAPHICS_TEXTURE_H


#include <cstdint>

#include "../utils/Math.h"

enum TextureFormat {
    RGBA,
    RGB
};

class Texture {
    float* data;
    int width, height;

    void LoadImage(const char* filePath);
public:
    explicit Texture(const char* filePath);
    ~Texture();

    int GetWidth() const;
    int GetHeight() const;

    vec4f Sample(const uint32_t u, const uint32_t v) const;
};


#endif //ADVANCEDGRAPHICS_TEXTURE_H
