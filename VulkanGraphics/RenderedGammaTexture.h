#pragma once
#include "Texture.h"

class RenderedGammaTexture : public Texture
{
private:
    void CreateTextureImage(VulkanEngine& engine);
    void CreateTextureView(VulkanEngine& engine);
    void CreateTextureSampler(VulkanEngine& engine);
public:
    RenderedGammaTexture();
    RenderedGammaTexture(VulkanEngine& engine);
    ~RenderedGammaTexture();

    void RecreateRendererTexture(VulkanEngine& engine);
};