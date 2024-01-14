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

vec4f Texture::Sample(const uint32_t u, const uint32_t v) const {
    uint32_t skyIdx = (u + v * width) % (width * height);

    if(skyIdx > width * height) {
        int a = 0;
    }

    return vec4f(data[skyIdx * 3], data[skyIdx * 3 + 1], data[skyIdx * 3 + 2], 1.0f) * 0.65f;
}

void Texture::LoadImage(const char *filePath) {
    int channels = 0;

    data = stbi_loadf(filePath, &width, &height, &channels, 3);
}

int Texture::GetWidth() const {
    return width;
}

int Texture::GetHeight() const {
    return height;
}

