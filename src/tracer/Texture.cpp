//
// Created by Ian on 8-1-2024.
//

#define STB_IMAGE_IMPLEMENTATION

#include "Texture.h"
#include "stb_image.h"

Texture::Texture(const char *filePath) {
    LoadImage(filePath);
}

Texture::~Texture() {
    free(data);
}

vec4f Texture::Sample(const uint32_t &u, const uint32_t &v) const {
    uint32_t skyIdx = (u + v * width) % (width * height);

    return vec4f(data[skyIdx * channels], data[skyIdx * channels + 1], data[skyIdx * channels + 2], 1.0f);
}

void Texture::LoadImage(const char *filePath) {
    data = stbi_loadf(filePath, &width, &height, &channels, 3);
}

int Texture::GetWidth() const {
    return width;
}

int Texture::GetHeight() const {
    return height;
}

void Texture::ClampToneMap(float clampValue) {
    for (int i = 0; i < width * height * 3; ++i) {
        data[i] = std::min(data[i], clampValue);
    }
}

