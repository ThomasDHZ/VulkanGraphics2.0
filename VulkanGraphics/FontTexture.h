#pragma once
#include "Texture.h"
#include <string>

class FontTexture : public Texture
{
private:
    void CreateTextTexture(void* GlyphData, uint32_t width, uint32_t height);
    void CreateTextureView();
    void CreateTextureSampler();
public:
    FontTexture();
    FontTexture(void* GlyphData, uint32_t width, uint32_t height);
    ~FontTexture();
};