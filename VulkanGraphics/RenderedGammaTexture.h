#pragma once
#include "Texture.h"

class RenderedGammaTexture : public Texture
{
private:
    void CreateTextureImage(VulkanEngine& renderer);
    void CreateTextureView(VulkanEngine& renderer);
    void CreateTextureSampler(VulkanEngine& renderer);
public:
    RenderedGammaTexture();
    RenderedGammaTexture(VulkanEngine& renderer);
    ~RenderedGammaTexture();

    void RecreateRendererTexture(VulkanEngine& renderer);
};