#pragma once
#include "Texture.h"

class RenderedGBufferNormalTexture : public Texture
{
private:
    void CreateTextureImage(VulkanEngine& engine);
    void CreateTextureView(VulkanEngine& engine);
    void CreateTextureSampler(VulkanEngine& engine);
public:
    RenderedGBufferNormalTexture();
    RenderedGBufferNormalTexture(VulkanEngine& engine);
    ~RenderedGBufferNormalTexture();

    void RecreateRendererTexture(VulkanEngine& engine);
};