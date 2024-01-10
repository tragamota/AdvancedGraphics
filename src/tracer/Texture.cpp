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

vec4f Texture::Sample(const float u, const float v) const {
    uint8_t skyIdx = u + v * width;

    return vec4f(data[skyIdx * 4], data[skyIdx * 4 + 1], data[skyIdx * 4 + 2], data[skyIdx * 4 + 3]);
}

void Texture::LoadImage(const char *filePath) {
    int channels = 0;

    data = stbi_loadf(filePath, &width, &height, &channels, 4);
}

const int Texture::GetWidth() const {
    return width;
}

const int Texture::GetHeight() const {
    return height;
}

