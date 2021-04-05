#pragma once
#include "Texture.h"

class RenderedGBufferAlbedoTexture : public Texture
{
private:
    void CreateTextureImage(VulkanEngine& engine);
    void CreateTextureView(VulkanEngine& engine);
    void CreateTextureSampler(VulkanEngine& engine);
public:
    RenderedGBufferAlbedoTexture();
    RenderedGBufferAlbedoTexture(VulkanEngine& engine);
    ~RenderedGBufferAlbedoTexture();

    void RecreateRendererTexture(VulkanEngine& engine);
};