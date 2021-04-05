#pragma once
#include "Texture.h"

class RenderedGBufferPositionTexture : public Texture
{
private:
    void CreateTextureImage(VulkanEngine& engine);
    void CreateTextureView(VulkanEngine& engine);
    void CreateTextureSampler(VulkanEngine& engine);
public:
    RenderedGBufferPositionTexture();
    RenderedGBufferPositionTexture(VulkanEngine& engine);
    ~RenderedGBufferPositionTexture();

    void RecreateRendererTexture(VulkanEngine& engine);
};