#pragma once
#include "Texture.h"

class RendereredTexture : public Texture
{
private:
    void CreateTextureImage(VulkanEngine& engine);
    void CreateTextureView(VulkanEngine& engine);
    void CreateTextureSampler(VulkanEngine& engine);
public:
    RendereredTexture();
    RendereredTexture(VulkanEngine& renderer, unsigned int textureID);
    ~RendereredTexture();
};

