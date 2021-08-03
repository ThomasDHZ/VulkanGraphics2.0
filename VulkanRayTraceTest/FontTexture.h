#pragma once
#include "Texture.h"
#include <string>

class FontTexture : public Texture
{
private:
    void CreateTextTexture(std::shared_ptr<VulkanEngine> engine, void* GlyphData, uint32_t width, uint32_t height);
    void CreateTextureView(std::shared_ptr<VulkanEngine> engine);
    void CreateTextureSampler(std::shared_ptr<VulkanEngine> engine);
public:
    FontTexture();
    FontTexture(std::shared_ptr<VulkanEngine> engine, void* GlyphData, uint32_t width, uint32_t height);
    ~FontTexture();
};