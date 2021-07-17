#pragma once
#include "Texture.h"
#include <string>

class TextTexture : public Texture
{
private:
    void CreateTextTexture(VulkanEngine& engine, void* GlyphData, uint32_t width, uint32_t height);
    void CreateTextureView(VulkanEngine& engine);
    void CreateTextureSampler(VulkanEngine& engine);
public:
    TextTexture();
    TextTexture(VulkanEngine& engine, void* GlyphData, uint32_t width, uint32_t height);
    ~TextTexture();
};